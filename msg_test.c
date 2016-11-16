// strategy 2 
#ifndef VERBOSE
#define VERBOSE 1
#endif

#include "msgs.h"
#include<stdlib.h> // for random number generator

#define NUM_CLIENTS 1000 
#define NUM_SERVERS 100  // cannot be more than NUM_PORTS (20)
#define MAX_MESSAGE_LETTER 10 // each letter of the message word (word length = 10) 
							  // won't be more than this value.
#define SLEEP_LENGTH 0

int clientid = 0;
int serverid = 0;

// assumes no line breaks. only prints message. 
void print_message (struct message_t *message)
{
	int i = 0;
	printf(" [");
	for(i = 0; i < MESSAGE_LENGTH; i++)
	{
		printf(" %d ",message->data[i]);   // print message.
	}
	printf(" ]");
}

void create_message(struct message_t *message, int client_id)
{
	int i;
	for(i = 1; i<MESSAGE_LENGTH; i ++)
	{
		message->data[i] = rand() % MAX_MESSAGE_LETTER;  
	}		
	message->sender = client_id;	
}

void client()
{
	int cid = clientid++;	
	int i;
	struct message_t message;	
	int served_by;	
	while(1)
	{
		served_by = rand() % (NUM_SERVERS);  // pick a server randomly.	
		create_message(&message, cid); 		//create a new message
		printf("%d- Client %d sending message to port %d :",Curr_Thread->id,cid,served_by);
		print_message(&message); 		
		printf("\n");		
		send(&message,served_by); // send to port of server (which is the server id itself)
		sleep(SLEEP_LENGTH);
	}
}

void server()
{
	int sid = serverid++;
	int i;
	struct message_t message;
	while(1)
	{
		// recieve a new message 
		receive(&message, sid);
		printf("%d- Server %d recieved message from client %d:",Curr_Thread->id,sid,message.sender);
		print_message(&message);
		printf("\n");
		sleep(SLEEP_LENGTH);
	}
}

void main()
{
	srand(time(NULL));
	int i;
	NewQueue(&ReadyQ);
   	initialize_ports();
	for (i=0; i< NUM_SERVERS; i++)
	{
		printf("Creating server %d\n", i);
    	start_thread(server);  // start all servers (only one for now)
	}		   
	for (i =0 ; i < NUM_CLIENTS; i ++)
	{
		printf("Creating client %d\n",i);		
    	start_thread(client);   // start all clients
	}
	run();
	while (1);
}   
