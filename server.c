#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// #include <sys/sendfile.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/uio.h>

#define MAX_PENDING 10							//Number of clients single time
#define MAX_SIZE    4096						//Buffer size
#define PORT    	3000

int write_response(int sockid, int i);

char* parse_req(int sockid, int size, char str[size]){
   	
   	/*--- Method Check ---*/
	if(strncmp(str, "GET ", 4)){
		write_response(sockid, 501);
		return NULL;
	}
	
	/*--- Protocol/Version Check ---*/
   	char* version = malloc(11);
   	version[11] = '\0';
   	char* str_temp = strchr(str+4, ' ');
   	strncpy(version, str_temp+1, 10);
   	if(strcmp(version, "HTTP/1.1\r\n")){
   		write_response(sockid, 400);
   		return NULL;
   	}
   	
   	/*--- URI Path ---*/
   	int len = strchr(str+4, ' ') - str - 4;
   	char* path = malloc(len+1);
   	strncpy(path, str+4, len);
   	path[len] = '\0';

   	return path;
}

int write_response(int sockid, int i){
	char buffer[4096];
	if(i == 501)
	{
		strcpy(buffer, "HTTP/1.1 501 Not Implemented\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-length: 105\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-Type: text/html\r\n\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<html>\n<head>\n<title>Not Implemented</title>\n</head>\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<body>\n<p>501 Not Implemented</p>\n</body>\n</html>\r\n");
  		write(sockid, buffer, strlen(buffer));
	}
	else if(i == 400)
	{
		strcpy(buffer, "HTTP/1.1 400 Bad Request\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-length: 97\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-Type: text/html\r\n\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<html>\n<head>\n<title>Bad Request</title>\n</head>\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<body>\n<p>400 Bad Request</p>\n</body>\n</html>\r\n");
  		write(sockid, buffer, strlen(buffer));
	}
	else if (i == 404)
	{
		strcpy(buffer, "HTTP/1.1 404 Not Found\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-length: 93\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-Type: text/html\r\n\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<html>\n<head>\n<title>Not Found</title>\n</head>\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<body>\n<p>404 Not Found</p>\n</body>\n</html>\r\n");
  		write(sockid, buffer, strlen(buffer));
	}
	else if (i == 500)
	{
		strcpy(buffer, "HTTP/1.1 500 Internal Server Error\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-length: 117\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-Type: text/html\r\n\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<html>\n<head>\n<title>Internal Server Error</title>\n</head>\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<body>\n<p>500 Internal Server Error</p>\n</body>\n</html>\r\n");
  		write(sockid, buffer, strlen(buffer));
	}
	return 0;
}

int main(int argc, char * argv[]) {
	int sockid, new_sockid;   					
	struct sockaddr_in sin;						
	int status;
	pid_t  pid;
	char send_buf[MAX_SIZE], recv_buf[MAX_SIZE];						
	socklen_t len;
	
	/*--- Checking number of arguments ---*/
  	if(argc!=1){
		fprintf(stderr, "argc\n");
		return 1;
	}

	/*--- Setting values of sockaddr_in ---*/
  	bzero((char *)&sin, sizeof(sin));
  	bzero((char *)send_buf, MAX_SIZE);			
  	bzero((char *)recv_buf, MAX_SIZE);			
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = htons(INADDR_ANY);

	/*--- Creating a socket ---*/
	if((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Error: socket");
		return 1;
	}
	
	/*--- Binding socket with ip and port ---*/
	if(bind(sockid, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("Error: bind");
		return 1;
	}

	/*--- Socket listening for a connection ---*/
	if((status = listen(sockid, MAX_PENDING)) != 0){
		perror("Error: listen");
		return 1;
	}

	printf("HTTP Server listening on Port %d\n", PORT);

	while(1){
		/*	Waiting to accept connection from a client  */
		if((new_sockid = accept(sockid, (struct sockaddr *)&sin, &len)) < 0) {
			perror("Error: accept");
			break;
		} 
		FILE *fsp = fdopen(new_sockid, "r+b");
		while(1){
			printf("waiting to receive\n");
			bzero((char *)recv_buf, MAX_SIZE);
			status = recv(new_sockid, recv_buf, MAX_SIZE, 0);
			
			if(status < 0) printf("recv() error\n");
			else if(status == 0) printf("Client Disconnected\n");
			
			// fputs(recv_buf, stdout);	

			printf("received request:\n %s\n", recv_buf);

			char* file_path = parse_req(new_sockid, strlen(recv_buf), recv_buf);

			if(file_path == NULL) continue;

			FILE *file = fopen(file_path+1,"rb");
			if (!file){
		    	printf("no such file exists\n");
		    	write_response(new_sockid, 404);
		    	continue;
		    } 
		    
		    printf("File exists\n");

		    fseek(file, 0L, SEEK_END);
		    int count = ftell(file);
		    fseek(file, 0L, SEEK_SET);

		    char *extension = strrchr(file_path, '.');

		    char buf_size[20];
		    sprintf(buf_size, "%d", count);

		    strcpy(send_buf, "HTTP/1.1 200 OK\r\n");
	  		write(new_sockid, send_buf, strlen(send_buf));
	  		strcpy(send_buf, "Content-length: ");
	  		strcat(strcat(send_buf, buf_size), "\r\n");
	  		printf("%s\n", send_buf);
	  		write(new_sockid, send_buf, strlen(send_buf));
	  		if(!strcmp(".txt", extension)) 
	  		{
	  			strcpy(send_buf, "Content-Type: text/plain\r\n\r\n");
	  		}
	  		else if (!strcmp(".html", extension) || !strcmp(".htm", extension))
	  		{
	  			strcpy(send_buf, "Content-Type: text/html\r\n\r\n");
			}
			else if (!strcmp(".gif", extension))
			{
				strcpy(send_buf, "Content-Type: image/gif\r\n\r\n");
			}
			else if (!strcmp(".jpeg", extension) || !strcmp(".jpg", extension))
	  		{
	  			strcpy(send_buf, "Content-Type: image/jpeg\r\n\r\n");
			}
			else if (!strcmp(".pdf", extension))
			{
				strcpy(send_buf, "Content-Type: Application/pdf\r\n\r\n");
			}
			printf("%s\n", send_buf);
	  		write(new_sockid, send_buf, strlen(send_buf));
	  		
	  		bzero((char *)send_buf, MAX_SIZE);
			
			/*	Sending file content  */
			int r_ret;
			while((r_ret = fread(send_buf, 1, MAX_SIZE, file)) > 0) {
			    status = fwrite(send_buf, 1, r_ret, fsp);
			    printf("characters sent:%d\n", status);
			   	if(status == 0)	{
					perror("Error: send file content");
					return 1;
				}
			    fflush(fsp);
			    bzero((char *)send_buf, MAX_SIZE);
			}
			fclose(file);
		}

		fclose(fsp);
    	close(new_sockid);	
	}
}