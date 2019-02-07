#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/inotify.h>

#include <fcntl.h>
#include <errno.h>
#include <linux/in.h>
#include <sys/socket.h>

#define BUFSZ   16384

//Global entities used by udp socket
char ctrl_ipaddr[25];
struct sockaddr_in saddr;
struct sockaddr_in caddr;
socklen_t clen;
int ssd;

/*
 *
 * Open UDP socket at any port available and transfer data to receiver opened at 5000 
 * Transferred data to hardcoded IP and hardcoded Port number
 * Need to add dynamic IP feature,i.e. getting Controller IP from neto-cli or somewhere else 
 *
 */

static int socket_init()
{
        //=========================================== Socket stage
        int sport,ret;
        ssd = socket(AF_INET,SOCK_DGRAM,0);
        if(ssd<0)
        {
                perror("Socket init failed\n");
                exit(1);
        }
        //========================================== Bind stage, Sender port and ip address
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(0);
        saddr.sin_addr.s_addr = INADDR_ANY;
        bzero(&saddr.sin_zero,sizeof(saddr.sin_zero));

        //printf("%s and %x\n",*ctrl_ipaddr,&ctrl_ipaddr);

        ret = bind(ssd,(struct sockaddr *)&saddr,sizeof(saddr));
        if(ret<0)
        {
                perror("Bind failed\n");
                exit(2);
        }
	//=========================================== Receiver port and ip address
        caddr.sin_family = AF_INET;
        caddr.sin_port = htons(5000);//*ptr);
        //caddr.sin_addr.s_addr = inet_addr("192.168.0.107");//(argv[1]);
        caddr.sin_addr.s_addr = inet_addr(&ctrl_ipaddr[0]);
        bzero(&caddr.sin_zero,sizeof(caddr.sin_zero));

        clen = sizeof(caddr);

        return 0;
}


static void errexit(char *s) 
{
        fprintf(stderr, "%s\n", s);
        exit(1);
}

int main(int argc,char *argv[]) 
{
        int ifd, wd, count, n, ret;
	FILE *fd;
        char buf[BUFSZ],file_buffer[500];
        struct inotify_event *ev;

	if(argc != 2)
        {
                perror("Too few arguments\n");
                exit(1);
        }

	strcpy(ctrl_ipaddr,argv[1]);
	printf("%s %s\n",argv[1],ctrl_ipaddr);

	//================================================================================================================
	//Inotify Init
	//================================================================================================================
	//Get fd of new inotify instance
        ifd = inotify_init();
        if (ifd < 0)
	{
                errexit("cannot obtain an inotify instance");
	}

        //Add new watch, by providing inotify fd, file path and notify action
        wd = inotify_add_watch(ifd, "/home/osboxes/VBS/opencapwap-SDN-Coupler/", IN_MODIFY|IN_CREATE|IN_DELETE);
        if (wd < 0)
        {        
                errexit("cannot add inotify watch");
        }
	
        //================================================================================================================
        //UDP Socket Init
        //================================================================================================================
	//Initialize UDP Socket [Get socket descriptor, Provide receiver's port and ip address]
        ret = socket_init();
        if(ret != 0)
        {
        	perror("UDP Socket Initialization Failed\n");
        	exit(2);
        }

	//FOREVER LOOP
        while (1) 
	{
		//In relay-to-controller code [READER CODE]
		//Step1: Check for Capwap-AC lock file
		//Step2: Create Relay lock file; if step1 false goto step2, else goto step1 
		//Step3: Check for Capwap-AC lock file; if step3 false proceed, else goto step5
		//Step4: DO WORK in critical section
		//Step5: Delete Relay lock file and sleep and goto step1
		//
		//NOTE:
		//Sleep in step5 will avoid spl race condition
		//Seq =>both check together and create respective lock files and check again and goto step5 and
		//	both check together ........<repeat forever>........
		//
		//So reader sleep will allow writer to proceed with WORK and avoid repeat forever cycle


		//Similarly in Capwap code [WRITER CODE]
		//Step1: Check for Relay lock file
                //Step2: Create Capwap-AC lock file; if step1 false goto step2, else goto step1
                //Step3: Check for Relay lock file; if step3 false proceed, else goto step5 
                //Step4: DO WORK in critical section
                //Step5: Delete Relay lock file and goto step1

	
		//Transfer multiple inotify structure to buf
                n = read(ifd, buf, sizeof(buf));
                if (n <= 0)
		{
                        errexit("read problem");
		}
		
                count = 0;
                while (count < n) 
		{
			//Read one inotify structure to ev
                        ev = (struct inotify_event *) &buf[count];
                        
			//Name of file under observation
			if (ev->len)
                        {
			      	// printf("file %s %s\n", ev->name,
                              	//         (ev->mask & IN_CREATE) ? "created" :
                              	//         (ev->mask & IN_DELETE) ? "deleted" :
                              	//         "modified");
                        	
				if( (strcmp(ev->name,"buffer.xml") == 0) && (ev->mask & IN_MODIFY) )
				{
					printf("Sent file to controller over UDP\n");
					fd = open("/home/osboxes/VBS/opencapwap-SDN-Coupler/buffer.xml", O_RDWR);
					printf("File descp=%d\n",fd);
		        		do	
			        	{
						//Read common file.
                        	                ret=read(fd,file_buffer,500);
						printf("read return=%d %s\n",ret, strerror(errno));
						
						//Transfer data to controller, over UDP.		
                        			int nbytes = sendto(ssd, file_buffer, strlen(file_buffer), 0,(struct sockaddr*)&caddr,clen);
                        			//printf("DEBUG: no of bytes=%d data=%s\n",nbytes,file_buffer);	     
                        			if(nbytes < 0)
                        			{
                        			        perror("Send failed");
                                			exit(3);
                        			}
                        
                			}while(ret>0);
					
					//if (fd != NULL)
					{
            					close(fd);
					}
				}
			}
			else
			{
                                printf("unexpected event - wd=%d mask=%d\n",
                                       ev->wd, ev->mask);
			}

			//Goto next inotify structure
                        count += sizeof(struct inotify_event) + ev->len;
                }

        }//FOREVER LOOP ENDS HERE

}

