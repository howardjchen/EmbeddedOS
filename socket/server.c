/*
    C socket server example, handles multiple clients using threads
*/
 
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
 

char client_message[2000];

//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    
    pthread_t sniffer_thread[10];
    int pthread_index = 0; 

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread[pthread_index] , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
        pthread_index++;
        break;
    }

    memset(&client_message,0,strlen(client_message));
    while(1)
    {
        printf("Enter message for client : ");
        scanf("%s" , client_message);
    }

     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    free(new_sock);
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
     //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message;
    int RecieveMessage; 
    int i;
    int MessageIndex = 0;
    int tmp;
    char buffer[10];
    char temp[2000];
    memset(&temp,0,strlen(temp));
     
    while(1)
    {
        if(strcmp(temp,client_message) != 0) //changed
        {
            //Send some data
            if( send(sock , client_message , strlen(client_message) , 0) < 0)
            {
                puts("Send failed");
                return 0;
            }
            strcpy(temp,client_message);
        }

        //write(sock , client_message , strlen(client_message));
        //RecieveMessage = atoi(&client_message[0]);
        //printf("rec = %d from client%d\n",RecieveMessage,sock);
        //printf("rec = %s from client%d\n",client_message,sock);
    }

     
    /*if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }*/
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}
