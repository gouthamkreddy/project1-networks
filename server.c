#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>

#define MAX_PENDING 10							//Number of clients single time
#define MAX_SIZE    4096						//Buffer size

char* allMonths[]= {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char* allWeeks[]= {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

int write_response(int sockid, int i, int conn);

int check_connection(int size, char str[size]){
	/*-- Check the Connection status in the request ---*/
	char* temp;
	temp = strstr(str, "connection:");
	if (!strncmp(str+12, "keep-alive", 10)) return 1;
	if (!strncmp(str+12, "close", 5)) return 2;
	return 0;
}

char* parse_req(int sockid, int size, char str[size]){
   	/*-- Check connection ---*/
	int conn = check_connection(size, str);

   	/*--- Method Check ---*/
	if(strncmp(str, "GET ", 4)){
		write_response(sockid, 501, conn);
		return NULL;
	}
	
	/*--- Protocol/Version Check ---*/
   	char* version = malloc(9);
   	version[8] = '\0';
   	char* str_temp = strchr(str+4, ' ');
   	strncpy(version, str_temp+1, 8);
   	if(strcmp(version, "HTTP/1.1")){
   		write_response(sockid, 400, conn);
   		return NULL;
   	}
   	
   	/*--- URI Path ---*/
   	if(strncmp(str+4, "/", 1)){
		write_response(sockid, 400, conn);
		return NULL;
	}
   	int len = strchr(str+4, ' ') - str - 4;
   	if(len > 1024)
   	{
   		write_response(sockid, 400, conn);
   		return NULL;
   	}
   	char* path = malloc(len+1);
   	strncpy(path, str+4, len);
   	path[len] = '\0';

   	return path;
}

int write_response(int sockid, int i, int conn){
	char buffer[4096];

	if(i == 501)
	{
		strcpy(buffer, "HTTP/1.1 501 Not Implemented\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-length: 105\r\n");
  		write(sockid, buffer, strlen(buffer));
  		time_t t = time(NULL);
	    struct tm *tm = localtime(&t);
	    sprintf(buffer, "Date: %s, %d %s %d %d:%d:%d GMT\n", allWeeks[tm->tm_wday], tm->tm_mday, allMonths[tm->tm_mon], tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
		write(sockid, buffer, strlen(buffer));
		if (conn == 2)
			strcpy(buffer, "Connection: close\r\n");
		else if(conn == 1 || conn == 0)
			strcpy(buffer, "Connection: keep-alive\r\n");		  		
		write(sockid, buffer, strlen(buffer));
		strcpy(buffer, "server: MyCServer\r\n");
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
  		time_t t = time(NULL);
	    struct tm *tm = localtime(&t);
	    sprintf(buffer, "Date: %s, %d %s %d %d:%d:%d GMT\n", allWeeks[tm->tm_wday], tm->tm_mday, allMonths[tm->tm_mon], tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
		write(sockid, buffer, strlen(buffer));
		if (conn == 2)
			strcpy(buffer, "Connection: close\r\n");
		else if(conn == 1 || conn == 0)
			strcpy(buffer, "Connection: keep-alive\r\n");		  		
		write(sockid, buffer, strlen(buffer));
		strcpy(buffer, "server: MyCServer\r\n");
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
  		time_t t = time(NULL);
	    struct tm *tm = localtime(&t);
	    sprintf(buffer, "Date: %s, %d %s %d %d:%d:%d GMT\n", allWeeks[tm->tm_wday], tm->tm_mday, allMonths[tm->tm_mon], tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
		write(sockid, buffer, strlen(buffer));
		if (conn == 2)
			strcpy(buffer, "Connection: close\r\n");
		else if(conn == 1 || conn == 0)
			strcpy(buffer, "Connection: keep-alive\r\n");		  		
		write(sockid, buffer, strlen(buffer));
		strcpy(buffer, "server: MyCServer\r\n");
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
  		time_t t = time(NULL);
	    struct tm *tm = localtime(&t);
	    sprintf(buffer, "Date: %s, %d %s %d %d:%d:%d GMT\n", allWeeks[tm->tm_wday], tm->tm_mday, allMonths[tm->tm_mon], tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
		write(sockid, buffer, strlen(buffer));
		if (conn == 2)
			strcpy(buffer, "Connection: close\r\n");
		else if(conn == 1 || conn == 0)
			strcpy(buffer, "Connection: keep-alive\r\n");		  		
		write(sockid, buffer, strlen(buffer));
		strcpy(buffer, "server: MyCServer\r\n");
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

int write_response_ok(int sockid, char* send_buf, FILE *file, char* extension, int conn)
{
	
	/*--- Sending 200 OK Response ---*/
	strcpy(send_buf, "HTTP/1.1 200 OK\r\n");
	write(sockid, send_buf, strlen(send_buf));
	
	/*--- Content-length ---*/
	fseek(file, 0L, SEEK_END);
	int count = ftell(file);
	fseek(file, 0L, SEEK_SET);
	char buf_size[20];
	sprintf(buf_size, "%d", count);
	strcpy(send_buf, "Content-Length: ");
	strcat(strcat(send_buf, buf_size), "\r\n");
	write(sockid, send_buf, strlen(send_buf));

	/*--- Content-Type ---*/
	if(!strcmp(".txt", extension)) 
		strcpy(send_buf, "Content-Type: text/plain\r\n");
	else if (!strcmp(".html", extension) || !strcmp(".htm", extension))
		strcpy(send_buf, "Content-Type: text/html\r\n");
	else if (!strcmp(".gif", extension))
		strcpy(send_buf, "Content-Type: image/gif\r\n");
	else if (!strcmp(".jpeg", extension) || !strcmp(".jpg", extension))
		strcpy(send_buf, "Content-Type: image/jpeg\r\n");
	else if (!strcmp(".pdf", extension))
		strcpy(send_buf, "Content-Type: Application/pdf\r\n");
	write(sockid, send_buf, strlen(send_buf));

	/*--- Date ---*/
	time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(send_buf, "Date: %s, %d %s %d %d:%d:%d GMT\n", allWeeks[tm->tm_wday], tm->tm_mday, allMonths[tm->tm_mon], tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
	write(sockid, send_buf, strlen(send_buf));

	/*--- Server ---*/
	strcpy(send_buf, "server: MyCServer\r\n");
	write(sockid, send_buf, strlen(send_buf));

	/*--- Connection ---*/
	if (conn == 2)
		strcpy(send_buf, "Connection: close\r\n\r\n");
	else if(conn == 1 || conn == 0)
		strcpy(send_buf, "Connection: keep-alive\r\n\r\n");		  		
	write(sockid, send_buf, strlen(send_buf));
	if (conn == 2)
		return 1;
	else if(conn == 1 || conn == 0)
		return 2;

	return 2;

}	

int main(int argc, char * argv[]) {
	int sockid, new_sockid;   					
	struct sockaddr_in sin;						
	int status;
	pid_t  pid;
	char send_buf[MAX_SIZE], recv_buf[MAX_SIZE];						
	socklen_t len;
	char line[1000], *dir;
	int port;
	uid_t uid = 1000;
	
	/*--- Checking number of arguments ---*/
  	if(argc!=1){
		fprintf(stderr, "No arguments required\n");
		return 1;
	}

	/*--- Read Config File and set PORT and DIR ---*/
	FILE *fp = fopen ("server.conf", "rb");
	while(fgets(line, 1000, fp) != NULL)
   	{
		if(!strncmp(line, "port", 4))
		{
			if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
			else line[strlen(line)] = '\0';
			port = atoi(line+7);
		}
		else if (!strncmp(line, "dir", 3))
		{
			if(line[strlen(line)-1] == '\n') line[strlen(line)-2] = '\0';
			else line[strlen(line)-1] = '\0';
			dir = line + 7;
		}
	}
    fclose(fp);  

    if (chdir(dir)) printf("error in chdir %d\n", errno);
    if (chroot(dir)) printf("error in chroot %d\n", errno);

	/*--- Setting values of sockaddr_in ---*/
  	bzero((char *)&sin, sizeof(sin));
  	bzero((char *)send_buf, MAX_SIZE);			
  	bzero((char *)recv_buf, MAX_SIZE);			
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htons(INADDR_ANY);

	/*--- Creating a socket ---*/
	if((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Error: socket");
		return 1;
	}

	int enable = 1;
    if (setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed");

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

	printf("HTTP Server listening on Port %d\n", port);

	while(1){
		/*--- Waiting to accept connection from a client ---*/
		if((new_sockid = accept(sockid, (struct sockaddr *)&sin, &len)) < 0)
		{
			perror("Error: accept");
			continue;
		} 
		printf("New Connection Created\n");

		pid = fork();
		if(pid == 0)
		{	
			close(sockid);
			FILE *fsp = fdopen(new_sockid, "r+b");
			while(1){
				printf("waiting---");
				bzero((char *)recv_buf, MAX_SIZE);
				status = recv(new_sockid, recv_buf, MAX_SIZE, 0);
				if(status < 0)
				{
					perror("Error: recv\n");	
					write_response(new_sockid, 500, 0);
					continue;
				} 
				else if(status == 0)
				{
					printf("Client Disconnected\n");
					break;
				} 

				char log_buf[1000];
				time_t t = time(NULL);
    			struct tm *tm = localtime(&t);
    			sprintf(log_buf, "Date: %s, %d %s %d %d:%d:%d GMT\tIP:%d.%d.%d.%d\t", allWeeks[tm->tm_wday], tm->tm_mday, allMonths[tm->tm_mon], tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(sin.sin_addr.s_addr&0xFF), (int)((sin.sin_addr.s_addr&0xFF00)>>8), (int)((sin.sin_addr.s_addr&0xFF0000)>>16), (int)((sin.sin_addr.s_addr&0xFF000000)>>24));
    			int temp_len = strstr(recv_buf, "\r\n") - recv_buf;
    			strcat(strncat(log_buf, recv_buf, temp_len),"\n");

    			FILE *logp = fopen("log.txt", "ab");
				fwrite(log_buf, 1, strlen(log_buf), logp);
				fclose(logp);

				char* file_path = parse_req(new_sockid, strlen(recv_buf), recv_buf);
				if(file_path == NULL) continue;
				printf("request: %s---", file_path);

				/*--- Handling '/' case in URI ---*/
				if (file_path[strlen(file_path)-1] == '/')
				{
					strcat(file_path, "index.html");
				}

				int conn = check_connection(strlen(recv_buf), recv_buf);

				/*--- Checking File ---*/
				FILE *file = fopen(file_path+1, "rb");
				if (!file){
			    	printf("!file\n");
			    	write_response(new_sockid, 404, conn);
			    	continue;
			    } 
			    printf("file\n");
			    char *extension = strrchr(file_path, '.');
			    
			    /*--- Check for extension ---*/
			    int ret;
			    if(extension == NULL)
			    {
			    	write_response(new_sockid, 501, conn);
			    	continue;
			    }
			    if(!strcmp(".jpeg", extension) || !strcmp(".jpg", extension) || !strcmp(".html", extension) || !strcmp(".htm", extension) || !strcmp(".txt", extension) || !strcmp(".gif", extension) || !strcmp(".pdf", extension))
			    {
			    	ret = write_response_ok(new_sockid, send_buf, file, extension, conn);
			    }
			    else
			    {
			    	write_response(new_sockid, 501, conn);
			    	continue;
			    }
			    /*--- Exiting if close comes in connection ---*/
			    if (ret == 1) break;              
			
				/*--- Sending file content ---*/
		  		bzero((char *)send_buf, MAX_SIZE);
				int r_ret;
				while((r_ret = fread(send_buf, 1, MAX_SIZE, file)) > 0) {
				    status = fwrite(send_buf, 1, r_ret, fsp);
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
			exit(0);
			printf("Connection Closed\n");
		}
		
    	close(new_sockid);	
	}
}









