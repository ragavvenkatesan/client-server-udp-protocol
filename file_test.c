/*	DMOS Project 5

Names: Dhanashree Adhikari, Ragav Venkatesan

--This is an implementation of file transer using message passing.
--The file is transferred in chunks of 40 bytes (10 integers)--configurable.
--The server enforces filename length is <= 15 and that the file size is approximately
--1 MB(configurable)
--Number of clients is passed as an argument
--Number os servers supported is currently 1

// After Compiling Run as ./a.out 3 file file file 
// Args: number of clients file1 file2... file
*/ 
  
#ifndef VERBOSE
#define VERBOSE 1
#endif

#include "msgs.h"


#define NUM_SERVERS 1  // cannot be more than 1 at the moment.
#define SLEEP_LENGTH 1
#define NUM_CLIENTS_SERVED 3  // simultaneously serve 
#define FILENAME_LENGTH 15
#define FILE_EXTENSION_LENGTH 7
#define MAX_FILE_SIZE 1000000

int NUM_CLIENTS = 0;
int client_list = 0;
int id = 0;
char *filenames[NUM_PORTS]; // be on the safe side
int server_list_top = 0;
int server_list[NUM_SERVERS];

int clients_running = 0;
int service_complete = 0;

struct service_module
{
	FILE* file;
	int bytesread;
	int id;
	int lock;
};

void client_retry()
{
	yield();
}

void client_exit()
{
	clients_running --;
	if(clients_running <=0)
	{
		service_complete = 1;
	}
	thread_exit();
}

void client()
{
	clients_running ++;
	int cid = id++;	
	FILE *fp;
	int i;
	struct message_t message,reply;	
	int served_by;
	char *filename = filenames[client_list++]; 
	printf("%s\n",filename);
	initialize_port(cid);
	int bytesread = MESSAGE_LENGTH;
		
	// client is locked to server at the moment. This need to change.	
	served_by = server_list[rand() % (NUM_SERVERS)];  // pick a server randomly.				
	while(1)
	{
		
		// Initiate transfer
		message.sender = cid;
		message.flag = 0;
		message.length = 0;
		
		send(&message, served_by);

		// Wait for Server
		receive(&message, cid);

		if(message.flag == -1)
		{
			printf("Server is Busy.. Will retry later\n");
			client_retry();
			continue;
		}
		else
		{
			printf("Cleint %d port is now open\n",cid);
		}
		
		
		// Send File Name
		message.length = strlen(filename);
		message.sender = cid;
		message.flag = 1;
		strcpy(message.data, filename);
		send(&message,served_by);
		// Wait For Server
		receive(&message, cid);
		if(message.flag == -1)
		{
			printf("Inappropriate file length\n");
			client_exit();
		}
		else
		{
			printf("Client %d file is approved for transfer\n",cid);
		}
		// Send file content
		printf("Sending file content\n");
		fp = fopen(filename, "rb");
		while(bytesread == MESSAGE_LENGTH)
		{
			message.sender = cid;
			message.flag = 2;
			bytesread = fread((void *)&message.data,1,MESSAGE_LENGTH,fp);
			message.length = bytesread;
			send(&message,served_by);
			sleep(SLEEP_LENGTH);
			receive(&reply, cid);
			if(reply.flag == -1)
			{
				printf("File size exceeded client %d exiting!\n", cid);
				client_exit();
			}			
		}
		fclose(fp); // send to port of server (which is the server id itself)
		message.flag = -1;
		message.length = 0;
		send(&message, served_by);
		client_exit();
	}
}

void server()
{
	int sid = id++;
	server_list[server_list_top++] = sid;
	int i, client_count = 0, fpcount = 0;
	
	// client_count is number of clients being served.
	// fpcount is pointer to files being served at the moment.
	// fp is file pointer
	
	struct service_module clients[NUM_CLIENTS_SERVED];
	int current_client = 0;
	int free_id;
	char filename[FILENAME_LENGTH + FILE_EXTENSION_LENGTH] = "";
	struct message_t message, reply;
	initialize_port(sid);
	while(1)
	{
		// recieve a new message 
		if(service_complete == 1)
		{
			thread_exit();
		}
		receive(&message, sid);
		switch(message.flag)
		{
			case 0:   // new client Request
				if(client_count < NUM_CLIENTS_SERVED)
				{
					client_count ++;
					reply.flag = 1;  // accepted
					for (i =0; i < NUM_CLIENTS_SERVED; i ++)
					{
						if(clients[i].lock == 0)
						{
							free_id = i;
							break;
						}
					}
					clients[free_id].id = message.sender; //init a service module
					clients[free_id].lock = 1; 
					clients[free_id].bytesread = 0;
					//
				}
				else
				{
					reply.flag = -1;  // declined
				}
				reply.length = 0;   
				send(&reply, message.sender);

			break;
			
			case 1:     // accepted Request recv filename
				for(i = 0; i < NUM_CLIENTS_SERVED; i++)
				{
					if(clients[i].id == message.sender)
					{
						current_client = i;
						break;
					}
				}	
								
				if(message.length > FILENAME_LENGTH)
				{
					
					client_count--;
					reply.flag = -1;   // reject client
					clients[current_client].lock =0;
				}
				else
				{				
					message.data[message.length] = '\0';
					strcpy(filename, message.data);
					strcat(filename, ".server");
					clients[current_client].file = fopen(filename, "w");
					reply.flag = 1;
				}
				reply.length = 0;
				send(&reply, message.sender);
			break;
			case 2:   // transferring file content
				for(i = 0; i < NUM_CLIENTS_SERVED; i++)
				{
					if(clients[i].id == message.sender)
					{
						current_client = i;
						break;
					}
				}
				reply.flag = 1;
				reply.length = 0;
				clients[current_client].bytesread += message.length;
				if(clients[current_client].bytesread > MAX_FILE_SIZE)// Max file size check
				{
					fclose(clients[current_client].file);
					clients[current_client].lock = 0;	
					client_count -- ;
					reply.flag = -1;
				}
				else				
					fwrite(message.data, 1, message.length, clients[current_client].file);
				send(&reply, message.sender);
			break;
			case -1:
				for(i = 0; i < NUM_CLIENTS_SERVED; i++)
				{
					if(clients[i].id == message.sender)
					{
						current_client = i;
						break;
					}
				}			
				fclose(clients[current_client].file);
				clients[current_client].lock = 0;	
				client_count -- ;	
			break;			
		}
		printf("%d- Server %d recieved message from client %d:",Curr_Thread->id,sid,message.sender);
		print_message(&message);
		printf("\n");
		sleep(SLEEP_LENGTH);
	}
}

void main(int argc, char *argv[])
{
	int i;
	if(argc<2)
	{
		printf("Please supply some job to do\n");
		exit(1);
	}	
	NewQueue(&ReadyQ);
	for (i =0 ; i < NUM_SERVERS; i ++)
	{
		printf("Creating Server %d\n",i);
    	start_thread(server);   // start all clients
	}
	NUM_CLIENTS = atoi(argv[1]);
	for (i =0 ; i < NUM_CLIENTS; i ++)
	{
		printf("Creating client %d\n",i);
		filenames[i] = argv[i+2];	
    		start_thread(client);   // start all clients
	}
	run();
	while (1);
}   
