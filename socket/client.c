/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <stdlib.h> 
 
int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    int read_size;
    char message[1000] , server_reply[2000];
    char Dispatch[4] = "quit";
    int cmd = 911;
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
     
    //keep communicating with server
    while( (read_size = recv(sock , server_reply , 2000 , 0)) > 0 )
    {
        printf("rec = %s\n",server_reply);

        if(cmd == atoi(server_reply))
        {
            printf("Client disconnected\n");
            memset(&server_reply,0,strlen(server_reply));
            break;
        }
        else if(read_size == 0)
        {
            puts("Client disconnected");
            fflush(stdout);
        }
        else if(read_size == -1)
            perror("recv failed");

        memset(&server_reply,0,strlen(server_reply));
    }

    shutdown(sock,SHUT_RDWR);

    return 0;
}
