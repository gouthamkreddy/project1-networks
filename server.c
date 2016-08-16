#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PENDING 10							//Number of clients single time
#define MAX_SIZE    4096						//Buffer size
#define PORT    	3001


// int parse(int size, char str[size]){
// 	Request = Request-Line
//              *(( general-header
//               | request-header ) CRLF)
//              CRLF
//              [ message-body ]
//     Request-Line = Method SP Request-URI SP HTTP-Version CRLF
//     Method = "GET"
//     HTTP-Version = "HTTP/1.1"
//     Request-URI = "*" | absoluteURI | abs_path | authority
//     // (Files are indicated by an absolute path (abs_path), 
//     // such as “/index.html” (a specific file relative to the
//     // server’s base directory) or such as “/images/" or “/” 
//     // (directories relative to the server's base directory).)
//     Connection = "Connection" ":" 1#(connection-token) connection-token = token
//     request-header = Accept                   
//                       | Accept-Charset           
//                       | Accept-Encoding          
//                       | Accept-Language          
//                       | Authorization            
//                       | Expect               
//                       | From                 
//                       | Host                 
//                       | If-Match             
//                       | If-Modified-Since    
//                       | If-None-Match        
//                       | If-Range             
//                       | If-Unmodified-Since  
//                       | Max-Forwards         
//                       | Proxy-Authorization  
//                       | Range                
//                       | Referer              
//                       | TE                   
//                       | User-Agent           
//     CR             = <US-ASCII CR, carriage return (13)>
//     LF             = <US-ASCII LF, linefeed (10)>
//     SP             = <US-ASCII SP, space (32)>
//     CRLF		   = CR LF

// }

// {
// 	response
// 	HTTP/1.1 200 OK
// 	200 OK
// 	400 Bad Request
//  	404 Not Found
// 	500 Internal Server Error
// 	501 Not Implemented
// }

char* parse_req(int sockid, int size, char str[size]){
   	
   	/*--- Method Check ---*/
	if(strncmp(str, "GET ", 4)){
		write_response(sockid, 501);
	}
	
	/*--- Protocol/Version Check ---*/
   	char* version = malloc(11);
   	version[11] = '\0';
   	char* str_temp = strchr(str+4, ' ');
   	strncpy(version, str_temp+1, 10);
   	if(strcmp(version, "HTTP/1.1\r\n")){
   		write_response(sockid, 400);
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
  		strcpy(buffer, "Server: Dynamic Thread\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-length: 105\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "Content-Type: text/html\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<html>\n<head>\n<title>Not Implemented</title>\n</head>\r\n");
  		write(sockid, buffer, strlen(buffer));
  		strcpy(buffer, "<body>\n<p>501 Not Implemented</p>\n</body>\n</html>\r\n");
  		write(sockid, buffer, strlen(buffer));
	}
	else if(i == 400)
	{
		strcpy(send_buf, "HTTP/1.1 400 Bad Request\r\n");
  		write(new_sockid, send_buf, strlen(send_buf));
  		strcpy(send_buf, "Server: Dynamic Thread\n");
  		write(new_sockid, send_buf, strlen(send_buf));
  		strcpy(send_buf, "Content-length: 97\r\n");
  		write(new_sockid, send_buf, strlen(send_buf));
  		strcpy(send_buf, "Content-Type: text/html\r\n");
  		write(new_sockid, send_buf, strlen(send_buf));
  		strcpy(send_buf, "\r\n");
  		write(new_sockid, send_buf, strlen(send_buf));
  		strcpy(send_buf, "<html>\n<head>\n<title>Bad Request</title>\n</head>\r\n");
  		write(new_sockid, send_buf, strlen(send_buf));
  		strcpy(send_buf, "<body>\n<p>400 Bad Request</p>\n</body>\n</html>\r\n");
  		write(new_sockid, send_buf, strlen(send_buf));
	}
	else if (i == 2)
	{
		/* code */
	}
	return 0;
}

int main(int argc, char * argv[]) {
	int sockid, new_sockid;   					//sockets
	struct sockaddr_in sin;						//sockaddr
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
		
		bzero((char *)recv_buf, MAX_SIZE);
		status = recv(new_sockid, recv_buf, MAX_SIZE, 0);
		
		if(status < 0){
			printf("recv() error\n");
		}else if(status == 0){
			printf("Client Disconnected\n");
		}else{
			fputs(recv_buf, stdout);
		}

		char* file_path = parse_req(new_sockid, strlen(recv_buf), recv_buf);
		// printf("%s\n", file_path);
		printf("strlen: %d", strlen("<html>\n<head>\n<title>Bad Request</title>\n</head>\r\n<body>\n<p>400 Bad Request.</p>\n</body>\n</html>\r\n"));

		// strcpy(send_buf, "HTTP/1.1 400 Bad Request\r\n");
  // 		write(new_sockid, send_buf, strlen(send_buf));
  // 		strcpy(send_buf, "Server: Dynamic Thread\n");
  // 		write(new_sockid, send_buf, strlen(send_buf));
  // 		strcpy(send_buf, "Content-length: 97\r\n");
  // 		write(new_sockid, send_buf, strlen(send_buf));
  // 		strcpy(send_buf, "Content-Type: text/html\r\n");
  // 		write(new_sockid, send_buf, strlen(send_buf));
  // 		strcpy(send_buf, "\r\n");
  // 		write(new_sockid, send_buf, strlen(send_buf));
  // 		strcpy(send_buf, "<html>\n<head>\n<title>Bad Request</title>\n</head>\r\n");
  // 		write(new_sockid, send_buf, strlen(send_buf));
  // 		strcpy(send_buf, "<body>\n<p>400 Bad Request</p>\n</body>\n</html>\r\n");
  // 		write(new_sockid, send_buf, strlen(send_buf));

		// strcpy(send_buf,"HTTP/1.1 200 OK\nContent-length: 47\nContent-Type: text/html\n\n<html><body><H1>Hello buddy</H1></body></html>");    
		// status = send(new_sockid,&send_buf, sizeof(send_buf),0);
		// status = write(new_sockid,&send_buf,sizeof(send_buf));
		
    	close(new_sockid);	
	}
}