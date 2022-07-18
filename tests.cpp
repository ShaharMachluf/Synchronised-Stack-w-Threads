#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 3490
#define SERVER_IP "127.0.0.1"
struct sockaddr_in serverAddress;

int create_sock(){
        int sock = socket(AF_INET, SOCK_STREAM, 0); //open socket
        if (sock == -1) {
            perror("socket");
            return -1;
        } 
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(PORT);
        int rval = inet_pton(AF_INET, (const char*)SERVER_IP, &serverAddress.sin_addr);//convert to binary 
        if (rval <= 0)
        {
            printf("inet_pton() failed");
            return -1;
        }
        return sock;
}

int main(){
        int sock1 = create_sock();
        int sock2 = create_sock();
        char ans[1024];
        char command[1024];
        if(connect(sock1, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){ //connect to server
            printf("connect() failed with error code : %d",errno);
            return 0;
        }
        memset(ans, 0, 1024);
        memset(command, 0, 1024);
        strcpy(command, "PUSH hello");
        sleep(2);
        send(sock1, command, sizeof(command), 0);
        recv(sock1, ans, sizeof(ans), 0);
        assert(strcmp(ans, "sent") == 0);//check push
        printf("correct!\n");
        if(connect(sock2, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){ //connect to server
            printf("connect() failed with error code : %d",errno);
            return 0;
        }
        memset(ans, 0, 1024);
        sleep(2);
        send(sock2, "TOP\n", sizeof("TOP\n"), 0);
        recv(sock2, ans, sizeof(ans), 0);
        assert(strcmp(ans, "hello") == 0);//check if a different socket see the same top
        printf("correct!\n");
        memset(ans, 0, 1024);
        sleep(2);
        send(sock2, "PUSH world", sizeof("PUSH world"), 0);
        recv(sock2, ans, sizeof(ans), 0);
        assert(strcmp(ans, "sent") == 0);//check that a second push from a different socket works
        printf("correct!\n");
        memset(ans, 0, 1024);
        sleep(2);
        send(sock1, "POP\n", sizeof("POP\n"), 0);
        recv(sock1, ans, sizeof(ans), 0);
        assert(strcmp(ans, "sent") == 0);//check if the first socket can pop what the second socket pushed
        printf("correct!\n");
        memset(ans, 0, 1024);
        sleep(2);
        send(sock2, "TOP\n", sizeof("TOP\n"), 0);
        recv(sock2, ans, sizeof(ans), 0);
        assert(strcmp(ans, "hello") == 0);//check that the pop worked
        printf("correct!\n");
        memset(ans, 0, 1024);
    // }
}