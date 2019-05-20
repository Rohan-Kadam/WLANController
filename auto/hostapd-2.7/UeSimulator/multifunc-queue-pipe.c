#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 


enum thread_state{
	SLEEP,
	WAKEUP,
	WAIT
};

enum queue_state{
	FULL,
	EMPTY,
	AVAILBLE
};

struct st{
	int consumer_s;
	int poissonprod_s;
	int realprod_s;
	int queue;
};

struct st status;
//#define DEBUG

#define max 5
char queue[max][80], data[80]= "hello";
pthread_mutex_t lock; 

//Determine the filled slots in queue
int qCounter;

//Queue Pointers
int front = -1, rear = -1, reply;
//front = -1; rear = -1;

//print status
void print_status(void)
{


	printf("\n\n\n============================================\n");
	system("date");
	printf("\n");
	if(status.consumer_s == WAKEUP)
	{
		printf("Consumer: WAKEUP\n");	
	}
	else if(status.consumer_s == SLEEP)
	{
		printf("Consumer: SLEEP\n");
	}
	else if(status.consumer_s == WAIT)
	{
		printf("Consumer: WAIT\n");
	}


	if(status.realprod_s == WAKEUP)
	{
		printf("Real Producer: WAKEUP\n");	
	}
	else if(status.realprod_s == SLEEP)
	{
		printf("Real Producer: SLEEP\n");
	}
	else
	{
		printf("Real Producer: WAIT\n");
	}


	if(status.poissonprod_s == WAKEUP)
	{
		printf("Simulated Producer: WAKEUP\n");	
	}
	else if(status.poissonprod_s == SLEEP)
	{
		printf("Simulated Producer: SLEEP\n");
	}
	else
	{
		printf("Simulated Producer: WAIT\n");
	}

	printf("\n");

	if(status.queue == EMPTY)
    {
    	printf("Queue: EMPTY\n");
	}
	else if(status.queue == AVAILBLE)
    {
    	printf("Queue: AVAILBLE\n");
	}
	else
    {
    	printf("Queue: FULL\n");
	}
	printf("Queue Filled Slots = %d (Max size of queue = %d)\n",qCounter,max);
	printf("============================================\n\n");

}
  
//exponential interval time for poisson
double ran_expo(double lambda)
{
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}

int insq(char queue[max][80],int *front, int *rear, char data[80])
{
	//exit(1);
	int f,r;
	
	//For Circular Queue
	if((*front == 0 && *rear == max-1) || (*rear == (*front-1)%(max-1)))	// queue full condition
		return(-1);


	else if (*front == -1) /* Insert First Element */{ 
        *front = *rear = 0; 
        strcpy(queue[*rear],data); 
		++qCounter;
		return 1;
    } 
	else if (*rear == max-1 && *front != 0) 
    { 
        *rear = 0; 
        strcpy(queue[*rear],data);
		++qCounter;
		return 1;
    } 
	else
    { 
        (*rear)++; 
        strcpy(queue[*rear],data);
		++qCounter;
		return 1;
    } 
}

int delq(char queue[max][80],int *front, int *rear, char data[80])
{
    if (*front == -1) {
    	//exit(1);  
        return -1; 
    } 
	strcpy(data,queue[*front]);
	--qCounter;
	if (*front == *rear) { 
		*front = -1; 
		*rear = -1; 
	} 
	else if (*front == max-1) 
		(*front) = 0; 
	else
		(*front)++; 
	return data;
}

void *del(void *myvar);
void *inst(void *myvar);
void *real (void *myvar);

// 3 threads running in main

int main (int argc, char **argv)
{ 

	char queue[max][80]; // data[80]= "hello";
	//  int front, rear, reply;
	//  front = rear = -1; 


	pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
   	int ref1, ref2, ref3;
    
	if (pthread_mutex_init(&lock, NULL) != 0) { 
		printf("\n mutex init has failed\n"); 
		return 1; 
	} 

  	status.consumer_s = WAKEUP;
	status.poissonprod_s = WAKEUP;
	status.realprod_s = WAKEUP;
	status.queue = EMPTY;

	print_status();

  	//ref1 = pthread_create(&thread1,NULL,inst,NULL);     
 	ref2 = pthread_create(&thread2,NULL,del,NULL);
  	ref3 = pthread_create(&thread3,NULL,real,NULL);
 
  	printf("Main function after pthread_create\n");

  	pthread_join(thread1, NULL);
  	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	//  pthread_exit(NULL);

  	printf(" 1nd thread ref1 = %d\n", ref1);
	printf(" 3rd thread ref3 = %d\n", ref3);
  	printf(" 2nd thread ref2 = %d\n", ref2);

  	pthread_mutex_destroy(&lock); 
  
  	return 0;
}


void *inst(void *myvar)
{
 	// char queue[max][80], data[80]= "hello"; 
  	//pthread_mutex_lock(&lock);

	//   pthread_mutex_lock(&lock); 

  	double lambda = 5;
  	int no_of_users=0;
  	double past_time,user_arrival_time; 
 
while(1) 
{
	past_time = time(0);
    user_arrival_time = ran_expo(lambda); // time arrival duration between two consecutive user arrivals
#ifdef DEBUG
    printf("WRITER: Poisson Delay = %lf\n",user_arrival_time);
	printf("WRITER: Poisson Delay Sleep -_-\n");
#endif
	//Poisson Delay	
	while(1)
   	{
        	if(time(0)-past_time >= user_arrival_time)      // run the while loop until system has run for (user_arrival_time) seconds, i.e, run the timer for that many seconds
                	break;                                  // break the while loop when (user_arrival_time) seconds has been passed
   	}

#ifdef DEBUG
	printf("WRITER: Poisson Delay Wakeup 0_0\n");
#endif

	//  printf("\nEnter string :");
	//  scanf("%s",data);

	pthread_mutex_lock(&lock);
#ifdef DEBUG
	printf("WRITER: Before Writing to queue, Queue Counter = %d front = %d rear =%d\n",qCounter,front,rear);
#endif

	reply=insq(queue,&front,&rear,data);

#ifdef DEBUG
	printf("WRITER: After Writing to queue, Queue Counter = %d front = %d rear =%d\n",qCounter,front,rear);
#endif	

	pthread_mutex_unlock(&lock);
  	if(reply == -1)
	{ 

#ifdef DEBUG		
		printf("WRITER: Queue is Full GOTO LONG SLEEP\n");
#endif 	
		status.poissonprod_s = SLEEP;
		status.queue = FULL;
		print_status();

 		usleep(1000000);


		status.poissonprod_s = WAKEUP;
		print_status();
		//rear = front;		
		
#ifdef DEBUG
 		printf("WRITER: WAKEUP FROM LONG SLEEP\n");
#endif
		//goto retry;
 		//break;
	}
  	else
  		{	
     		printf("WRITER:'%s' is inserted in queue.\n================> IN\n\n",data);


		status.poissonprod_s = WAKEUP;
		status.queue = AVAILBLE;
		print_status();	
    	}
 	} 

 	//pthread_mutex_unlock(&lock); 
 	return NULL;
}

void *del(void *myvar)
{

	// delete
 	//  char queue[max][80];
	/*  char *msg;
  	msg =  ( char *) myvar; // to convert the void pointer to a char pointer for reading the message
 	pthread_mutex_lock(&lock); 

  	printf("%s\n ",msg );

	*/
  	//pthread_mutex_lock(&lock); 
 	char * myfifo1 = "/tmp/MW2AP_FIFO";		// transapi downlink
  	char * myfifo2 = "/tmp/SimUeDL_FIFO";	// hostapd     "
  	
  	int a, fd1,fd2;
    char str1[80];

    mkfifo(myfifo1,0666);

	fd1 = open(myfifo1,O_RDWR);  
	fd2 = open(myfifo2,O_RDWR); 
	
//	sleep(1);	

	while(1) 
 	{
		pthread_mutex_lock(&lock); 
		//rear = qCounter;

#ifdef DEBUG
		printf("READER: Before Reading from queue, Queue Counter = %d front = %d rear =%d\n",qCounter,front,rear);   		
#endif

		reply = delq(queue, &front, &rear, data);

#ifdef DEBUG		
		printf("READER: After Reading from queue, Queue Counter = %d front = %d rear =%d\n",qCounter,front,rear);   		
#endif

		pthread_mutex_unlock(&lock);
   	
		if( reply == -1 )
  		{    

#ifdef DEBUG    
			printf("READER: Queue is Empty GOTO LONG SLEEP\n");
#endif
		status.consumer_s = SLEEP;
		status.queue = EMPTY;
		print_status();
		usleep(1000000);

		status.consumer_s = WAKEUP;
		print_status();
			
#ifdef DEBUG
			printf("READER: WAKEUP FROM LONG SLEEP\n");
#endif
  		}

   		else
		{
        	printf("READER: Deleted String from Queue is : %s\n================> OUT\n\n", reply);

			status.consumer_s = WAKEUP;
			print_status();
    		
    		char dataToBeWritten[50];
 			strcpy(dataToBeWritten,reply);

 			FILE *filePointer;
 
 			//filePointer = fopen ("mine.txt" , "w");
 			filePointer = fopen ("/etc/toMW.txt","w");

 			// check if this file pointer is null
 			// which maybe if the file does not exist

 			if ( filePointer == NULL )
    				printf ("READER: failed to open");
 			else{

    			printf("READER: file is opened.\n");
 			// write the dataToBeWritten into the file
    			if ( strlen ( dataToBeWritten ) > 0 ){
   					fputs(dataToBeWritten, filePointer );	// Write the data into the file
   				}

  				fclose(filePointer);			// closing the file using fclose()
#ifdef DEBUG	
	  			printf ("READER: Data successfully written in the file \n");
  				printf ("READER: the file in now closed\n\n");
#endif
  			}

  			//Wait for response on MW2AP pipe here

#ifdef DEBUG	
			printf("READER: Waiting for response from SDN Controller\n");
#endif
			status.consumer_s = WAIT;
			print_status();
    
			a = read(fd1, str1, 80); 
			
			status.consumer_s = WAKEUP;
			print_status();

#ifdef DEBUG	
			printf("READER: Received data from the SDN Controller\n");
#endif

    		//Send to simUeDl_FIFO if real ue response
  			write(fd2, str1, a);

#ifdef DEBUG	
  			printf("READER: Sent data to real UE\n");
#endif    		
  			//Ignore if simulated ue response
		}
	}//while 1 ends here

   //pthread_mutex_unlock(&lock); 
   return NULL;
}

// Pipe read and queue write ////

void *real (void *myvar)
{
	int fd1; 
  	char * myfifo = "/tmp/SimUeUL_FIFO";	// 
  	int a;

	fd1 = open(myfifo,O_RDONLY | O_NONBLOCK); 
	printf("SimUeUl_FIFO fd = %d\n",fd1);
	char str1[80], str2[80]; 
   	while (1) 
    {	 
		// First open in read only and read 
		a = read(fd1, str1, 80); 

		// Print the read string and close 
		if (a > 0)
		{
retry:
			printf("REAL WRITER: %s\n", str1); 
			pthread_mutex_lock(&lock);

#ifdef DEBUG	
			printf("REAL WRITER: Before Writing to queue, Queue Counter = %d front = %d rear =%d\n",qCounter,front,rear);
#endif
			reply=insq(queue,&front,&rear,str1);

#ifdef DEBUG	
			printf("REAL WRITER: After Writing to queue, Queue Counter = %d front = %d rear =%d\n",qCounter,front,rear);
#endif
			pthread_mutex_unlock(&lock);
			if(reply == -1)
			{
			

#ifdef DEBUG	
				printf("REAL WRITER: Queue is Full GOTO LONG SLEEP\n");
#endif

				status.realprod_s = SLEEP;
				status.queue = FULL;
				print_status();
		
				sleep(1);
				//rear = front;         
#ifdef DEBUG
				printf("REAL WRITER: WAKEUP FROM LONG SLEEP\n");
#endif				
				//break;
				status.realprod_s = WAKEUP;
				print_status();

				goto retry;
			}
			else{
				status.realprod_s = WAKEUP;
				status.queue = AVAILBLE;
				print_status();	
				printf("REAL WRITER:'%s' is inserted in queue.\n================> IN\n\n",data);
			}
		}

	}
	close(fd1); 
	return 0; 		
}