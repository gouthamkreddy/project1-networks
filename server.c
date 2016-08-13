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

int main(int argc, char * argv[]) {
	int sockid, new_sockid;   					//sockets
	struct sockaddr_in sin;						//sockaddr
	int status;
	char send_buf[MAX_LINE], recv_buf[MAX_LINE];						
	socklen_t len;
	/*	Checking number of arguments  */
  	if(argc!=2){
		fprintf(stderr, "argc\n");
		return 1;
	}

	/*	Setting values of sockaddr_in  */
  	bzero((char *)&sin, sizeof(sin));
  	bzero((char *)send_buf, MAX_SIZE);			
  	bzero((char *)recv_buf, MAX_SIZE);			
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argv[1]));
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

	while(1){
		/*	Waiting to accept connection from a client  */
		if((new_sockid = accept(sockid, (struct sockaddr *)&sin, &len)) < 0) {
			perror("Error: accept");
			break;
		} 

		printf("%s:%d connected\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
		
		FILE *fsp = fdopen(new_sockid, "r+b");

	
		bzero((char *)buf, sizeof(buf));			//Filling buf with zero valued bytes
			
		fread(buf, 1, sizeof(buf), fsp);
			

		fclose(fsp);
    	close(new_sockid);	
	}
}