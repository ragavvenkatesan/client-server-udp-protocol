#include "sem.h"

#define NUM_MESSAGES_PER_PORT 10
#define MESSAGE_LENGTH 10
#define NUM_PORTS 100

struct message_t
{
	int data[MESSAGE_LENGTH];
	int sender;
};

struct port_t 
{
	struct message_t messages[NUM_MESSAGES_PER_PORT];
	int in, out;
	struct Semaphore_t mutex,full,empty;
};

struct port_t port[NUM_PORTS];

void initialize_ports() 
{
	printf("Initializing Ports\n");
	int i;
	for(i=0; i<NUM_PORTS; i++)
	{
		InitSem(&port[i].mutex,1);
		InitSem(&port[i].full,NUM_MESSAGES_PER_PORT); 
		InitSem(&port[i].empty,0);
		port[i].in = 0;
		port[i].out = 0;
	}
}
	
void send(struct message_t *message, int port_id)
{
	int i;	
	P(&port[port_id].full);				
	P(&port[port_id].mutex);  // since we don't do pre-emptive, this mutex is useless.
		for(i = 0; i < MESSAGE_LENGTH; i++)  // loop and atomically copy message letter by letter.
		{
			port[port_id].messages[port[port_id].in].data[i] = message->data[i];
		}
		port[port_id].messages[port[port_id].in].sender = message->sender;
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
		for(i = 0; i < MESSAGE_LENGTH; i++) // loop over and atomically collect the entire message
		{
			message->data[i] = port[port_id].messages[port[port_id].out].data[i];
		}
		message->sender = port[port_id].messages[port[port_id].out].sender;
		port[port_id].out = (port[port_id].out + 1) % NUM_MESSAGES_PER_PORT;
	V(&port[port_id].mutex);
	V(&port[port_id].full);				
}
