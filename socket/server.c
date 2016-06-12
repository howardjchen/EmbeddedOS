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
#include <mysql/mysql.h>
#include <unistd.h>
 
#define Forward 7
#define Right 1
#define Left 2
#define Back 3
#define Stop 5
#define Start 6

char client_message[2000];

void finish_with_error(MYSQL*);
void *connection_handler(void *);   //the thread function
 


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    MYSQL *con;
    MYSQL_ROW row;
    MYSQL_RES *result;
    int Overlapcmd;

     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);    //Create socket
    if (socket_desc == -1)
        printf("Could not create socket");
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     

    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)  //Bind
    {
        perror("bind failed. Error");   //print the error message
        return 1;
    }
    puts("bind done");
     
    listen(socket_desc , 3); //Listen
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    //Accept and incoming connection
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
        puts("Handler assigned");
        pthread_index++;
        break;
    }    

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    memset(&client_message,0,strlen(client_message));

/**************************MySQL main thread*********************/
    con = mysql_init(NULL);
    if (con == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }  
    if (mysql_real_connect(con, "localhost", "root", "5168", "robotdata", 0, NULL, 0) == NULL) 
        finish_with_error(con);  

    while(1)
    {
        if (mysql_query(con, "SELECT * FROM signals")) 
            finish_with_error(con);

        result = mysql_store_result(con);
        if (result == NULL) 
            finish_with_error(con);
        
        row = mysql_fetch_row(result);

        if(strcmp(row[1],"Forward") == 0 && Overlapcmd != Forward)
        {
            sprintf(client_message,"Forward");
            Overlapcmd = Forward; 
        }
        else if(strcmp(row[1],"Right") == 0 && Overlapcmd != Right)
        {
            sprintf(client_message,"Right");
            Overlapcmd = Right;
        }
        else if(strcmp(row[1],"Left") == 0 && Overlapcmd != Left)
        {
            sprintf(client_message,"Left");
            Overlapcmd = Left;
        }
        else if(strcmp(row[1],"Back") == 0 && Overlapcmd != Back)
        {
            sprintf(client_message,"Back");
            Overlapcmd = Back;
        }
        else if(strcmp(row[1],"Stop") == 0 && Overlapcmd != Stop)
        {
            sprintf(client_message,"Stop");
            Overlapcmd = Stop;
        }
        else if(strcmp(row[1],"Start") == 0 && Overlapcmd != Start)
        {
            //sprintf(client_message,"Right");
            Overlapcmd = Start;
        }
        usleep(500000);
    }

     
    mysql_free_result(result);
    mysql_close(con);     
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
    }

    free(socket_desc);
     
    return 0;
}

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);        
}