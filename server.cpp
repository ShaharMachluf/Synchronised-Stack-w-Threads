/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "stack.cpp"

#define PORT 3490  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

pnode head = NULL;
pthread_mutex_t my_mutex;

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//thread function
void *start_thread(void* fd){
    int sock = *(int*)fd;
    int flag = 1, i=0, bytes_get = 0;
    char command[1024];
    char data[1024];
    while(flag){
        memset(data, 0, 1024);
        memset(command, 0, 1024);
        bytes_get = 0;
        while(bytes_get == 0){
            bytes_get = recv(sock, data, 1024, 0);//recieve command from the client
        }
        if(data[0] == '\0'){
            continue;
        }
        i=0;
        while (data[i] != '\0' && data[i]!= ' ')//move the command part (PUSH/POP/TOP) to "command"
        {
            command[i] = data[i];
            i++;
        }
        pthread_mutex_lock(&my_mutex);
        if(strcmp(command, "PUSH")==0){
            int j=0;
            i++;
            while(data[i]!='\0'){//move the data part (without the push) to "data"
                data[j]=data[i];
                j++;
                i++;
            }
            data[j] = '\0';
            push(data, &head);
            if (send(sock, "sent", sizeof("sent"), 0) == -1)//confirm it worked
                perror("send failed\n");
            pthread_mutex_unlock(&my_mutex);
            continue;
        }
        else if(strcmp(command, "POP\n")==0){
            if(head == NULL){
                if (send(sock, "ERROR: stack is empty", sizeof("ERROR: stack is empty"), 0) == -1)
                    perror("send failed\n");
                pthread_mutex_unlock(&my_mutex);
                continue;
            }
            pop(&head);
            if (send(sock, "sent", sizeof("sent"), 0) == -1)//confirm it worked
                perror("send failed\n");
            pthread_mutex_unlock(&my_mutex);
            continue;
        }
        else if(strcmp(command, "TOP\n")==0){
            if(head == NULL){
                if (send(sock, "ERROR: stack is empty", sizeof("ERROR: stack is empty"), 0) == -1)
                    perror("send failed\n");
                pthread_mutex_unlock(&my_mutex);
                continue;
            }
            memset(data, 0, 1024);
            strcpy(data, top(&head));
            if (send(sock, data, sizeof(data), 0) == -1)//send the data that the user asked for
                perror("send failed\n");
            pthread_mutex_unlock(&my_mutex);
            continue;
        }
        else if(strcmp(command, "EXIT")==0){
            pthread_mutex_unlock(&my_mutex);
            close(sock);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&my_mutex);
    }
}
int main(void)
{
    int sockfd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv, i=0;
    pthread_t threads[15];//assume maximum 15 threads working at the same time

        signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket
    int listeningSocket = -1, data_len = 0, clientSocket = -1;
    char buf[256];
    int flag = 0;
    socklen_t len = sizeof(buf);
    int sock = socket(AF_INET, SOCK_STREAM, 0);//create socket
    if (sock == -1) {
        perror("socket");
        return -1;
    }   
	 
    if((listeningSocket = socket(AF_INET , SOCK_STREAM , 0 )) == -1)
    {
        printf("Could not create listening socket : %d" ,errno);
    }
    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);  //network order
      
    // Bind the socket to the port with any IP at this port
    if (bind(listeningSocket, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code : %d" ,errno);
	// TODO: close the socket
        return -1;
    } 
    printf("Bind() success\n");

    // Make the socket listening; actually mother of all client sockets.
    if (listen(listeningSocket, 1) == -1) //1 is a Maximum size of queue connection requests
											//number of concurrent connections 
    {
	printf("listen() failed with error code : %d",errno);
	// TODO: close the socket
        return -1;
    }
    //Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");
      
    struct sockaddr_in clientAddress;  
    socklen_t clientAddressLen = sizeof(clientAddress);

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        int new_fd = accept(listeningSocket, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        if(pthread_create(&(threads[i]), NULL, &start_thread, (void*)&new_fd)!=0){//create thread
            printf("all the threads are taken");
        }
       i=(i+1)%15;
    }
    pthread_mutex_destroy(&my_mutex);
    return 0;
}
                                        