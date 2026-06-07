#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdlib.h>
#include<netdb.h>
#include<string.h>

#define LISTEN_BACKLOG 50

int main(){
	int socfd = socket(AF_INET ,SOCK_STREAM  ,0 );
	if(socfd < 0){ perror("Socket error"); 
	// exit(-1);
	}


	struct addrinfo  hint , *res;	
	memset(&hint , 0 , sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	
	if(getaddrinfo(NULL , "8080" , &hint , &res) != 0){
	    printf("GetAddrr fails to run ");
		exit(-1);
	}
	
	if(bind(socfd ,res->ai_addr ,res->ai_addrlen) < 0){
	perror("Bind Error");
	exit(-1);
	}
	//Listen to incoming conncetions !!
	 if(listen(socfd , LISTEN_BACKLOG) < 0){
		perror("Listen() error");
		exit(-1);
		};
		
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr);
int client_fd = accept(socfd, (struct  sockaddr *) &their_addr, &addr_size);
	if(client_fd< 0){
	perror("accept error");
	exit(-1);
	  };
	
	    int MAX_LEN = 1000;
		char http_request[MAX_LEN];
		memset(http_request, 0, MAX_LEN);
		int num_bytes = recv(client_fd ,http_request , MAX_LEN , 0);
		if(num_bytes < 0){
		perror("recv Error");
		}
		if(strncmp(http_request , "GET" , 3) == 0){
		printf("Received HTTP GET request :\n");
		    // Now we can responsed to HTTP GET request here ;
			// REad index.html here and then send it to client --> 
				
			    FILE *index_fd = fopen("index.html" , "r");
				char c;
				while((c = getc(index_fd)) != EOF){
				int dataSent = send(client_fd, &c ,1, 0);	
				}
				
		}
		
	close(socfd);
	close(client_fd);
		
	return 0;
}
