/**
 * @file 5gtMR-Relay-Wifi-MgmtFrame-Info.c
 * @author Rohan Kadam
 * @brief Reads and modifies child nodes content
 * @note gcc -Wall -I/usr/include/libxml2 -o relay-2 5gtMR-Relay-to-Controller-2.c -lxml2
 */

#include <libxml/parser.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/tree.h>
#include <stdbool.h>
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

#define TRUE 1
#define FALSE 0
#define DOCNAME "../buffer.xml"
#define BUFSZ   16384


//Global entities used by udp socket
char ctrl_ipaddr[25];
struct sockaddr_in saddr;
struct sockaddr_in caddr;
socklen_t clen;
int ssd;

unsigned char bm_bssid[50];
unsigned char bm_dest_addr[50];
unsigned char bm_source_addr[50]={0};
char buffer[100];
char head[10];
xmlChar head_count_string[50];
xmlChar empty_count_string[50];

static volatile int max_head_count = 2;
static volatile int head_count = 2;
static volatile bool Update_Buffer_Block = FALSE;




/** 
	FUNCTIONS TO PARSE XML BUFFER

Assumming an XML file with the following contents, but with multiple entries
<?xml version="1.0" encoding="UTF-8"?>
<init>
        <!-->Determines current position in buffer<-->
        <Head>0</Head>

        <!-->To avoid Buffer Overflow we can use this parameter<-->
        <Empty_Slots>3</Empty_Slots>
    
        <!-->Buffering m-SBI control packets<--> 
        <Buffer>

                <!-->*********************** BLOCK 0 ***********************<-->        
                <!-->Param 1: Buffer Position<-->
                <Buffer_Position>[0]</Buffer_Position>
                <!-->Param 2: CAPWAP control message from CAPWAP AC (Eg. Association,SNR,MAC)<-->       
                <CAPWAP_Msg_Param>EMPTY</CAPWAP_Msg_Param>
                <!-->Param 3: Status of buffer block (Eg. Waiting,Read,Empty)<-->
                <Complete>MODIFY</Complete>


                <!-->*********************** BLOCK 1 ***********************<-->
                <!-->Param 1: Buffer Position<-->
                <Buffer_Position>[1]</Buffer_Position>
                <!-->Param 2: CAPWAP control message from CAPWAP AC (Eg. Association,SNR,MAC)<-->       
                <CAPWAP_Msg_Param>EMPTY</CAPWAP_Msg_Param>
                <!-->Param 3: Status of buffer block (Eg. Waiting,Read,Empty)<-->
                <Complete>MODIFY</Complete>


                <!-->*********************** BLOCK 2 ***********************<-->
                <!-->Param 1: Buffer Position<-->
                <Buffer_Position>[2]</Buffer_Position>
                <!-->Param 2: CAPWAP control message from CAPWAP AC (Eg. Association,SNR,MAC)<-->       
                <CAPWAP_Msg_Param>EMPTY</CAPWAP_Msg_Param>
                <!-->Param 3: Status of buffer block (Eg. Waiting,Read,Empty)<-->
                <Complete>MODIFY</Complete>

        </Buffer>
</init>

*/

void my_crude_itoa()
{
        if(head_count == 0)
        {
                head_count_string[0] = '0';
                head_count_string[1] = '\0';

                empty_count_string[0] = '2';
                empty_count_string[1] = '\0';
        }
        else if(head_count == 1)
        {
                head_count_string[0] = '1';
                head_count_string[1] = '\0';
                
                empty_count_string[0] = '1';
                empty_count_string[1] = '\0';
        
        }
        else if(head_count == 2)
        {
                head_count_string[0] = '2';
                head_count_string[1] = '\0';
        
                empty_count_string[0] = '0';
                empty_count_string[1] = '\0';
        
        }
                        
}

static void parse_buffer(xmlDocPtr doc, xmlNodePtr cur)
{

        xmlChar *key;
        cur = cur->xmlChildrenNode;

        while (cur != NULL) 
		{
                
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"Buffer_Position"))) 
				{
                       	key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        if(strcmp(key,head)==0)
                        {
                                printf("-------------------->>>>>>>>>>>>>> BLOCK %s SELECTED <<<<<<<<<<<<<<--------------\n",key);
                                Update_Buffer_Block = TRUE;                
                        }
                        xmlFree(key);
                
                }                

                if ((!xmlStrcmp(cur->name, (const xmlChar *)"CAPWAP_Msg_Param"))) 
				{
                        if(Update_Buffer_Block == TRUE)
                        {
                        	//Read        
                        	key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        	printf(" Message at %s is %s\n",head,key);
                        	//copy to buffer here, which will be sent over UDP to Controller
                        	sprintf(buffer,"%s",key);
							xmlFree(key);

							Update_Buffer_Block = FALSE;
						}
                }                

                /*Remove this field*/	
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"Complete"))) 
                {
                	key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                    xmlFree(key);
                }
                cur = cur->next;
                
        }

}

static void parse_init(xmlDocPtr doc, xmlNodePtr cur)
{
        xmlChar *key;
        cur = cur->xmlChildrenNode;
        
        while (cur != NULL) 
        {
        	if ((!xmlStrcmp(cur->name, (const xmlChar *)"Head"))) 
        	{
            
            	key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            	sprintf(head,"%s",key);
            	xmlFree(key);
            	printf("Head: %s\n",head);
			}

			if ((!xmlStrcmp(cur->name, (const xmlChar *)"Empty_Slots"))) 
			{
                key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                printf("\nEmpty_Slots: %s\n",key);
                xmlFree(key);
            }
 		
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"Buffer"))) 
            {
                key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                printf("\nBuffer:\n");
                parse_buffer(doc, cur);
                xmlFree(key);
            }
            cur = cur->next;
        }
}

void buffer_main()
{
    xmlDocPtr doc;
    xmlNodePtr cur;
        
	printf("In xml code\n");
	
	doc = xmlParseFile(DOCNAME);
	if (doc == NULL) 
    {
        fprintf(stderr, "Document not parsed successfully. \n");
        return (0);
    }
	
	cur = xmlDocGetRootElement(doc);
    if (cur == NULL) 
    {
        fprintf(stderr, "empty document\n");
        xmlFreeDoc(doc);
        return (0);
    }

    if (xmlStrcmp(cur->name, (const xmlChar *)"init")) 
    {
        fprintf(stderr, "document of the wrong type, root node != config");
        cur = cur->xmlChildrenNode;
        xmlFreeDoc(doc);
        return (0);
    }
	
	parse_init(doc, cur);
	
	//Causes inotify to loop infinitely, do not uncomment
	//xmlSaveFormatFileEnc("./test/test.xml", doc, "UTF-8", 1);
	
}


/*
 * SOCKET INITIALIZATION
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
	//printf("%s %s\n",argv[1],ctrl_ipaddr);

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
    wd = inotify_add_watch(ifd, "/home/osboxes/VBS/opencapwap-SDN-Coupler/", IN_MODIFY);
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
	printf("Start\n");
	//FOREVER LOOP STARTS HERE
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
		//this is a blocking API
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
				printf("breakpoint\n");
		      	// printf("file %s %s\n", ev->name,
             	//         (ev->mask & IN_CREATE) ? "created" :
              	//         (ev->mask & IN_DELETE) ? "deleted" :
              	//         "modified");
                        	
				if( (strcmp(ev->name,"buffer.xml") == 0) && (ev->mask & IN_MODIFY) )
				{
					printf("Sent file to controller over UDP\n");
	
					//1.semaphore in shared mem
					
					//2.read/parse xml file
					buffer_main();
					
					//3.copy to buffer (do it while reading xml file)
					
					//4.proceed with transmitting buffer over udp
	        		//do	
		        	//{
						//Read common file.
       	                //ret=read(buffer,file_buffer,500);
						//printf("read return=%d %s\n",ret, strerror(errno));
	
						//Transfer data to controller, over UDP.		
    					int nbytes = sendto(ssd, buffer, strlen(buffer), 0,(struct sockaddr*)&caddr,clen);
                        //printf("DEBUG: no of bytes=%d data=%s\n",nbytes,file_buffer);	     
                        if(nbytes < 0)
                        {
                            perror("Send failed");
                        	exit(3);
                        }
                    //}while(ret>0);
					
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
			//printf("count=%d\n",count);
			//Goto next inotify structure
                        count += sizeof(struct inotify_event) + ev->len;
			//printf("count=%d\n",count);
                }
		
		//printf("\n\n\nOUTSIDE\n\n\n");
        }//FOREVER LOOP ENDS HERE

}

