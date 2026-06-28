#include <netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdlib.h>
#include<netdb.h>
#include<string.h>
#include"threadPool.h"
#include"lruCache.h"

#define LISTEN_BACKLOG 50

pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;


void init_thread_pool(int num_threads);
void add_task(int client_fd);
void destroy_thread_pool();

lruCache* cache;


char* read_file(const char* filename , size_t* file_size){
    FILE *fp = fopen(filename , "rb");
    if(fp == NULL) return NULL;
    fseek( fp , 0 , SEEK_END);
    *file_size = ftell(fp);
    rewind(fp);
    char* buffer = malloc(*file_size);
    fread(buffer , 1 , *file_size , fp);
    fclose(fp);
    return buffer;
}


void handle_client(int client_fd){
        sleep(10);
        int MAX_LEN = 1000;
		char http_request[MAX_LEN];
		memset(http_request, 0, MAX_LEN);
		int num_bytes = recv(client_fd ,http_request , MAX_LEN , 0);
		if(num_bytes < 0){
		perror("recv Error");
		};

	
		
		if(strncmp(http_request , "GET" , 3) == 0){
		printf("Received HTTP GET request :\n");
		    // Now we can responsed to HTTP GET request here ;
			// REad index.html here and then send it to client --> 
				char*header = "HTTP/1.1 200 OK\r\n"
				                "Content-Type: text/html\r\n"
								"Connection: close\r\n"
								"\r\n";
				send(client_fd , header , strlen(header) , 0);
			 //    FILE *index_fd = fopen("index.html" , "r");
				// if(index_fd == NULL){
				// printf("Fopen error");
				// close(client_fd);
				// return;
				// }
				// while((c = getc(index_fd)) != EOF){
				// int dataSent = send(client_fd, &c ,1, 0);	
				// }
				pthread_mutex_lock(&cache_mutex);
				CacheNode* entry = cache_get(cache , "/index.html");
				if(entry != NULL){
				printf("Cache HIT\n");
				send(client_fd , entry->content , entry->content_size , 0);
				}else{
				printf("Cache MISS\n");
				size_t size;
				char* file_data = read_file("index.html", &size);
				if(file_data == NULL){close(client_fd); return;};
				cache_put(cache , "/index.html" , file_data , size);
				send(client_fd , file_data , size , 0);
				free(file_data);
				pthread_mutex_unlock(&cache_mutex);
				};

				
		}
		
	close(client_fd); 
}

void* thread_function(void* arg){
    int client_fd = *(int*)arg;
    free(arg);

    printf(
        "Thread %lu | fd=%d \n",
        (unsigned long)pthread_self(),
        client_fd); 
    handle_client(client_fd);
    return NULL;
};


int main(){
    init_thread_pool(2);
    // cache = cache_init(5);

    
    int socfd = socket(AF_INET ,SOCK_STREAM  ,0 );
	if(socfd < 0){ perror("Socket error"); 
	exit(-1);
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
		printf("Listen() error");
		exit(-1);
	};
		
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr);

	
	
	while(1){
	    int client_fd = accept(socfd, (struct  sockaddr *) &their_addr, &addr_size);
		if(client_fd< 0){
		perror("accept error");
        continue;
		  };
	
		char ipstr[INET_ADDRSTRLEN];
		struct sockaddr_in *addr = (struct sockaddr_in *)&their_addr;
	  inet_ntop(AF_INET , &addr->sin_addr , ipstr , sizeof(ipstr));
		printf("Accepted Connection from %s fd=%d\n" ,ipstr , client_fd);	
		add_task(client_fd);
		// pthread_t thread1;
		// int *fd_ptr = malloc(sizeof(int));
		// *fd_ptr = client_fd;
		// if(pthread_create(&thread1, NULL, thread_function, fd_ptr) != 0){
		// perror("pthread_create Error");
		// close(client_fd);
		// free(fd_ptr);
		// continue;
		// };
		// pthread_detach(thread1);
	}
	destroy_thread_pool();
	// cache_destroy(cache);
	close(socfd);
	return 0;
}
