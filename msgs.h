#include "sem.h"
#include<stdlib.h> // for random number generator

#define NUM_MESSAGES_PER_PORT 1
#define MESSAGE_LENGTH 40
#define NUM_PORTS 10
#define MAX_MESSAGE_LETTER 10 // each letter of the message word (word length = 10) 
struct message_t
{
	char data[MESSAGE_LENGTH];
	int sender;	
	int length;
	int flag;
};

// assumes no line breaks. only prints message. 
void print_message (struct message_t *message)
{
	int i = 0;
	printf(" [");
	printf(" Flag:%d ",message->flag);
	printf(" Length:%d ",message->length);
	for(i = 0; i < message->length; i++)
	{
		printf("%c",message->data[i]);   // print message.
	}
	printf(" ]");
}

struct port_t 
{
	struct message_t messages[NUM_MESSAGES_PER_PORT];
	int in, out;
	struct Semaphore_t mutex,full,empty;
};

struct port_t port[NUM_PORTS];

void initialize_port( int port_id )
{
		InitSem(&port[port_id].mutex,1);
		InitSem(&port[port_id].full,NUM_MESSAGES_PER_PORT); 
		InitSem(&port[port_id].empty,0);
		port[port_id].in = 0;
		port[port_id].out = 0;
}

void initialize_all_ports() 
{
	printf("Initializing Ports\n");
	int i;
	for(i=0; i<NUM_PORTS; i++)
	{
		initialize_port(i);
	}
}

// copy constructor basically.
void copy_message(struct message_t *dst, struct message_t *src)
{
	int i = 0;
	for(i = 0; i < src->length ; i ++)
		{
			dst->data[i] = src->data[i];
		}
	dst->sender = src->sender;
	dst->flag = src->flag;
	dst->length = src->length;
}
	
void send(struct message_t *message, int port_id)
{
	P(&port[port_id].full);				
		P(&port[port_id].mutex);  // since we don't do pre-emptive, this mutex is useless.
			copy_message(&port[port_id].messages[port[port_id].in],message);
			port[port_id].in = (port[port_id].in + 1) % NUM_MESSAGES_PER_PORT;
		V(&port[port_id].mutex);
	V(&port[port_id].empty);
				
}

void receive(struct message_t *message, int port_id)
{
	// essentially a consumer routine
	int i;
	P(&port[port_id].empty);				
		P(&port[port_id].mutex);
			copy_message(message,&port[port_id].messages[port[port_id].out]);					
			port[port_id].out = (port[port_id].out + 1) % NUM_MESSAGES_PER_PORT;
		V(&port[port_id].mutex);
	V(&port[port_id].full);				
}
