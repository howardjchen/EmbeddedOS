#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
 

#define Queue_length 4

struct node
{
	int priority;
	int info;
	struct node *link;
}*HeadNode=NULL;

int dequeue[10000];
int elementNumber = 0;

void insert(int item, int item_priority);
void print();
int ChechEmpty();
void search(int);
int del();


//the thread function
void *connection_handler(void *);
 

void DequeueGO()
{
	int i;
	int n = 0;
	int element = 0;
	int search_element = 0;
	int priority = 0;
	FILE *fp1;
	FILE *fp2;
	FILE * rule;
	FILE *countfp;
	int num = 0;
	struct node *ptr;
	int cnt;
	int rulenumber[2];
	int rulepriority[2];
	char string[3];
	int indexx = 0;


	printf("============== good to go!! =====================\n");

	rule = fopen("rule.txt","r");
	while(fscanf(rule,"%s %d %s %s %s %d",&string[0],&rulepriority[indexx],&string[1],&string[2],&string[3],&rulenumber[indexx]) != EOF)
	{
		printf("number %d is priority %d\n",rulenumber[indexx], rulepriority[indexx]);
		indexx++;
	}


	countfp = fopen("data.txt","r");
	while(fscanf(countfp,"%d",&cnt) != EOF)
	{
		elementNumber++;
	}

	printf("number of data = %d\n",elementNumber );



	fp1 = fopen("data.txt","r");	
	for(i = 0; i < Queue_length; i++)
	{
		fscanf(fp1,"%d",&num);
		element = num;
		//printf("%d " ,element);
		if((element % rulenumber[0]) == 0)
        {
        	priority = 1;
        }
        else if( (element % rulenumber[1]) == 0 )
			priority = 2;
        else
        	if(i < 3)
				priority = 3;
			else
				priority = i;
        insert(element,priority);
	}


	ptr = HeadNode;
	i = 0;
	int priority_shift = elementNumber;
	int flag = 0;

	fp2 = fopen("data.txt","r");
	int start = 0;

	while( i < elementNumber )
	{
		flag = fscanf(fp2,"%d",&num);		
		if(start >= Queue_length)
		{
			dequeue[i] = del();//ptr -> info;
			printf("dequeue[%d] = %d  ",i,dequeue[i] );

			i++;
			priority_shift++;
			if(flag != -1)
			{
				element = num;
				printf("read : %d ",element );
				if((element % rulenumber[0] ) == 0)
				{
					priority = 1;
				}
				else if((element % rulenumber[1]) == 0)
					priority = 2;
				else 
					priority = priority_shift+1;
				insert(element,priority);
			}
			print();
		}
		else
		{
			start++;
		}
	}
}



int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock = 0;
    struct sockaddr_in server , client;
     
    DequeueGO();

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
    }

     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
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
    char client_message[2000];
    int RecieveMessage; 
    int i;
    int MessageIndex = 0;
    int tmp;
    char buffer[10];

          
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //write(sock , client_message , strlen(client_message));
        RecieveMessage = atoi(&client_message[0]);
    	printf("rec = %d from client%d\n",RecieveMessage,sock);

    	for (i = 0; i < elementNumber ; ++i)
    	{
    		if((dequeue[i] % RecieveMessage) == 0)
    		{
    			tmp = dequeue[i];
    			sprintf(buffer,"%d ",tmp);
    			//client_message[] = buffer;
    			//printf("%s \n", buffer );
    			write(sock , buffer , strlen(buffer));
    			//MessageIndex++;
    		}
    	}

    	//write(sock , client_message , strlen(client_message));

    }


    
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}


void insert(int item,int item_priority)
{
	struct node *tmp;
	struct node *p;

	tmp = (struct node*)malloc(sizeof(struct node));

	if(tmp == NULL)
	{
		printf("Memory not available\n");
		return;
	}
	
	tmp->info = item;
	tmp->priority = item_priority;

	if( ChechEmpty() || item_priority < HeadNode->priority )
	{
		tmp->link = HeadNode;
		HeadNode = tmp;
	}
	else
	{
		p = HeadNode;
		while( p->link != NULL && p->link->priority <= item_priority )
		p = p->link;
		tmp->link = p->link;
		p->link = tmp;
	}
}


int ChechEmpty()
{
	if( HeadNode == NULL )
		return 1;
	else
		return 0;
}

void print()
{
	struct node *ptr;
	ptr = HeadNode;
	if( ChechEmpty() )
		printf("Queue is empty");
	else
	{    
		printf("queue :  ");
		//printf("Priority     element\n");
	
		while(ptr != NULL)
		{
			printf("%d ",ptr->info);
			//printf("%5d        %5d\n",ptr->priority, ptr->info);
			ptr = ptr->link;
		}
	}
	printf("\n");
}

void search(int element)
{
	struct node *ptr;
	ptr = HeadNode;
	int flag = 0;

	while(flag == 0 && ptr != NULL)
	{
		if(element == ptr->info)
		{
			flag = 1;
		}
		else
		{
			ptr = ptr->link;
		}
	}

	if(flag == 0)
		printf("not found\n");
	else
		printf("found %d and priority is %d\n",ptr->info,ptr->priority );
}
int del()
{
	struct node *tmp;
	int item;
	if( ChechEmpty() )
	{
		printf("Queue Underflow\n");
		exit(1);
	}
	else
	{
		tmp = HeadNode;
		item = tmp -> info;
		HeadNode = HeadNode -> link;
		free(tmp);
	}
	return item;
}
