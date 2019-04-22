/**
 *
 * Compile using:  
 * Export path using: export LD_LIBRARY_PATH=<file-path>:$LD_LIBRARY_PATH
 * @file main.c
 * @author Rohan-Kadam
 * @date April 18, 2019
 * @brief This is main file of project.
 *
 * \mainpage Description
 * Add description here
 *  
 * \section library_sec libraries used in this Project 
 * \subsection library1
 * library1 description
 * more description
 *
**/


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

sigset_t set1,set2;

struct sigaction act1,act2;
//TODO: Add semaphores for handling flag 
//Since it is updated by multiple child processes.
int flag = 0;
/*
void sig_hdl()
{
	int ret,status;
	
	//while(1)
	//{
		//printf("%d\n",getpid());
		ret = waitpid(-1,&status,0);
		if(ret>0)
		{
			//printf("Parent %d\n",getpid());
			if(WIFEXITED(status))
			{
				if(WEXITSTATUS(status)==0)
				{
					printf("Normal Successful Termination <--------------\n");
				}
				else
				{
					printf("Normal Unsuccessful Termination <---------------\n");
				}
			}
			else
			{
				printf("Abnormal Termination <--------------\n");
			}
			flag++;
		}
		
		//else if(flag==5)
		//	{
		//		printf("All cleared bye!!!");
		//		exit(0);	
		//	}
	//}
}
*/
int main()
{
	int ret,i=0;
	int flags;

	int s2c, c2s; 
    char fifo_name1[] = "/tmp/fifo1";
    char fifo_name2[] = "/tmp/fifo2";
    char *pathname = "/tmp/theFIFO";
    char msg[80] = "Hello neto-cli";
    char buf[10];
    struct stat st;	
/*
	sigfillset(&set1);
	sigdelset(&set1,SIGCHLD);
	sigprocmask(SIG_SETMASK,&set1,&set2);
	
	act1.sa_handler=sig_hdl;	
	sigfillset(&act1.sa_mask);
	sigaction(SIGCHLD,&act1,&act2);
*/
	while(i++<5)
	{
		ret = fork();
		
		if(ret<0)
		{
			printf("Fork failed...!!\n");
			exit(1);
		}

		if(ret>0)
		{
			if(i == 1)
			{
				printf("Parent ppid %d and pid %d\n",getppid(),getpid());
			
				//FIFO
				//if (stat(fifo_name1, &st) != 0)
			    //    mkfifo(fifo_name1, 0777);
			
			if((mkfifo(pathname, S_IRWXU | S_IRWXG))<0){
		        printf("Could not create named pipe %s.\n", pathname);
		        printf("%s.\n", strerror(errno));
		        exit(EXIT_FAILURE);
		    }



			    if (stat(fifo_name2, &st) != 0)
		    	    mkfifo(fifo_name2, 0666);
				
				s2c= open(pathname, S_IRWXU | S_IRWXG);
				if(s2c < 0)
				{
					printf("FIFO OPEN FAILED\n");
					
				}	
    			c2s= open(fifo_name2, O_RDONLY | O_NONBLOCK);
			}	
			//sprintf();
			continue;	
		}
		
		if(ret==0)
		{
			printf("Child ppid %d and pid %d\n",getppid(),getpid());
			//while(1);
			if(i==1){

                  //ret = execl("/usr/bin/gcc", "gcc", 
                  //      "/home/desd/desd_labs/EOS/TEST/fork/fork3n.c","-o","1execl.out", NULL);
                  //if(ret<0) { perror("error in execl1"); exit(2); }
		
          }  
          if(i==2){

                  //ret = execl("/usr/bin/evince", "evince", 
                  //      "/home/desd/desd_labs/EOS/TEST/Linux_assignment2_aug2016.pdf", NULL);
                  //if(ret<0) { perror("error in execl1"); exit(2); }
          }
          if(i==3){

                  ret = execl("/usr/bin/gnome-calculator", "gnome-calculator", 
                         NULL);
                  if(ret<0) { perror("error in execl1"); exit(2); }
          } 
          if(i==4){

                  ret = execl("/usr/bin/gnome-terminal", "gnome-terminal","-e","/usr/local/bin/netopeer-cli /tmp/fifo1 /tmp/fifo2",
                      NULL);
                  if(ret<0) { perror("error in execl1"); exit(2); }
          } 
          if(i==5){

                  //ret = execl("/usr/bin/firefox", "firefox", 
                  //      "https://www.google.com","LINUX", NULL);
                  //if(ret<0) { perror("error in execl1"); exit(2); }

          	//	ret = execl("/usr/local/bin/netopeer-cli", "netopeer-cli", 
          	//		NULL);
                //if(ret<0) { perror("error in execl1"); exit(2); }

          } 
           printf("BYE");
           exit(0);
		}
	}	//while-loop ends here..
	
	if(ret>0)
	{
		ret = write(s2c, msg, strlen(msg)+1);
		printf("fd = %d msg = %s return value = %d\n",s2c,msg,ret);

		while(1)
		{
			//sigsuspend(&set1);
			//printf("%d\n",flag);
              		if(flag==5)
                        {
                                unlink(fifo_name1);
    							unlink(fifo_name2);
                                printf("All cleared bye!!!\n");
                                exit(0);        
                        }

                    
		}
	}	
}
