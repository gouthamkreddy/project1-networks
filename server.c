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
#define PORT    	3000					

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

char* filename(int size, char str[size]){
	char *firstline;
   	firstline = strchr(str, '\n');
   	int len = firstline - str + 1;
   	printf("\n\n%.*s\n\n", len, str);
   	printf("%d\n", len);
   	return str;
}

int main(int argc, char * argv[]) {
	int sockid, new_sockid;   					//sockets
	struct sockaddr_in sin;						//sockaddr
	int status;
	pid_t  pid;
	char send_buf[MAX_SIZE], recv_buf[MAX_SIZE];						
	socklen_t len;
	
	/*	Checking number of arguments  */
  	if(argc!=1){
		fprintf(stderr, "argc\n");
		return 1;
	}

	/*	Setting values of sockaddr_in  */
  	bzero((char *)&sin, sizeof(sin));
  	bzero((char *)send_buf, MAX_SIZE);			
  	bzero((char *)recv_buf, MAX_SIZE);			
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = htons(INADDR_ANY);

	/*	Creating a socket	*/
	if((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Error: socket");
		return 1;
	}
	
	/*	Binding socket with ip and port  */
	if(bind(sockid, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("Error: bind");
		return 1;
	}

	/*	Socket listening for a connection  */
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
		
		// parse(strlen(recv_buf), recv_buf);
		// printf("size:%d\n", strlen(recv_buf));
		//Check if string is valid or not

		filename(strlen(recv_buf), recv_buf);

		strcpy(send_buf,"HTTP/1.1 200 OK\nContent-length: 47\nContent-Type: text/html\n\n<html><body><H1>Hello buddy</H1></body></html>");    
		status = send(new_sockid,&send_buf,sizeof(send_buf),0);
		// status = write(new_sockid,&send_buf,sizeof(send_buf));

		printf("\nSTATUS:%d\n",status);
		      
		   // printf("\nSent : %s\n",xx);
		
		
    	close(new_sockid);	
	}
}