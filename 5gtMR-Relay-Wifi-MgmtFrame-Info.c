/**
 * @file 5gtMR-Relay-Wifi-MgmtFrame-Info.c
 * @author Rohan Kadam
 * @brief Reads and modifies child nodes content
 * @note gcc -Wall -I/usr/include/libxml2 -o 5gtMR-Relay-Wifi-MgmtFrame-Info 5gtMR-Relay-Wifi-MgmtFrame-Info.c -lxml2
 */
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/tree.h>
*/

#include "5gtMR-Relay-Wifi-MgmtFrame-Info.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

#define DOCNAME "../buffer.xml"

unsigned char bm_bssid[50];
unsigned char bm_dest_addr[50];
unsigned char bm_source_addr[50]={0};
char buffer[100];
xmlChar head_count_string[50];
xmlChar empty_count_string[50];

static volatile int max_head_count = 2;
static volatile int head_count = 2;
static volatile bool Update_Buffer_Block = FALSE;

/** Assumming an XML file with the following contents, but with multiple entries
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

        CWLog("Inside parse buffer");

        while (cur != NULL) {
                //                printf("Entering Buffer_Position-->\n");
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"Buffer_Position"))) {
                        //Read Previous Content
                        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf(" Buffer_Position: %s\n", key);
                        
                        Update_Buffer_Block = FALSE;
                        //printf("%d %s %d %s\n",strlen(key),key,strlen(head_count_string),head_count_string);
                        if(strcmp(key,head_count_string)==0)
                        {
                                printf("-------------------->>>>>>>>>>>>>> BLOCK %s SELECTED <<<<<<<<<<<<<<--------------\n",key);
                                Update_Buffer_Block = TRUE;                
                        }
                        else
                        {
                                printf("---------------------------------- BLOCK %s ----------------------------\n",key);                                
                        }                                
                        
                        xmlFree(key);
                
			//xmlNodeSetContent(cur,"MODIFY");

                        //Read Modified Content        
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf(" Buffer_Position changed to: %s\n", key);
                        xmlFree(key);

                }                

                //printf("Entering CAPWAP_Msg_Param-->\n");
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"CAPWAP_Msg_Param"))) {
                        //Read Previous Content
                        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf(" CAPWAP_Msg_Param: %s\n", key);
                        xmlFree(key);

                        if(Update_Buffer_Block == TRUE)
                        {
                        /*
                                strcat(bm_source_addr[0],":");
                                strcat(bm_source_addr[0],bm_source_addr[1]);
                                strcat(bm_source_addr[0],":");
                                strcat(bm_source_addr[0],bm_source_addr[2]);
                                strcat(bm_source_addr[0],":");
                                strcat(bm_source_addr[0],bm_source_addr[3]);
                                strcat(bm_source_addr[0],":");
                                strcat(bm_source_addr[0],bm_source_addr[4]);
                                strcat(bm_source_addr[0],":");
                                strcat(bm_source_addr[0],bm_source_addr[5]);
                                strcat(bm_source_addr[0],":");
                                strcat(bm_source_addr[0],bm_source_addr[6]);
                        */
                                //xmlNodeSetContent(cur,bm_source_addr);
                                xmlNodeSetContent(cur,buffer);              
                        }
                        
                        //Read Modified Content        
                        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf(" CAPWAP_Msg_Param changed to: %s\n", key);
                        xmlFree(key);

                }                

                //printf("Entering Complete-->\n");
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"Complete"))) {
                        //Read Previous Content
                        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf(" Complete: %s\n", key);
                        xmlFree(key);
                
                        //xmlNodeSetContent(cur,bm_bssid);

                        //Read Modified Content        
                        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf(" Complete changed to: %s\n", key);
                        xmlFree(key);
                                
                        printf("-------------------------------------------------------------------\n\n");
                                
                }
                cur = cur->next;
                
        }
}

static void parse_init(xmlDocPtr doc, xmlNodePtr cur)
{
        xmlChar *key;
        cur = cur->xmlChildrenNode;
        
        while (cur != NULL) {
                
                CWLog("Inside parse init");
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"Head"))) {
                        
                        if(head_count < max_head_count)
                        {
                                head_count++;
                        }
                        else
                        {
                                head_count = 0;
                        }
                        //itoa(head_count,head_count_string,10);
                        my_crude_itoa();
                        //printf("%s %c\n",head_count_string,head_count_string[0]);
                        
                        xmlNodeSetContent(cur,&head_count_string[0]);
                        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf("\nHead: %s\n",key);
                        xmlFree(key);
                
                }

                if ((!xmlStrcmp(cur->name, (const xmlChar *)"Empty_Slots"))) {
                        xmlNodeSetContent(cur,&empty_count_string[0]);
                        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf("\nEmpty_Slots: %s\n",key);
                        xmlFree(key);
        
                }
 		
                if ((!xmlStrcmp(cur->name, (const xmlChar *)"Buffer"))) {
                        key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                        printf("\nBuffer:\n");
                        parse_buffer(doc, cur);
                        xmlFree(key);
                }
                cur = cur->next;
                //cur = NULL;
        }
}

int buffer_main(unsigned char* bssid,unsigned char* dest_addr,unsigned char* source_addr)
{
        xmlDocPtr doc;
        xmlNodePtr cur;

        //Not needed, can be removed
        strcpy(bm_bssid,bssid);
        strcpy(bm_dest_addr,dest_addr);
        strcpy(bm_source_addr,source_addr);                
        
        CWLog("Received ---> %s <--- for XML Buffer update",bm_dest_addr);


        //CWLog("************* INSIDE NEW CODE BLOCK *************\n");
        
        if(strcmp(bm_dest_addr,"Association") == 0)        
        {        
                CWLog("######### Association received from %02x:%02x:...\n",bm_source_addr[0],bm_source_addr[1]);
        
                sprintf(buffer,"Association received from UE:%02x:%02x:%02x:%02x:%02x:%02x at AP:%02x:%02x:%02x:%02x:%02x:%02x\n",bm_source_addr[0],
                bm_source_addr[1],bm_source_addr[2],bm_source_addr[3],bm_source_addr[4],
                bm_source_addr[5],bm_bssid[0],bm_bssid[1],bm_bssid[2],bm_bssid[3],
                bm_bssid[4],bm_bssid[5]);
        }
        else
        {
                CWLog("Probe request received from %02x:%02x:...\n",bm_source_addr[0],bm_source_addr[1]);
        
                sprintf(buffer,"Probe request received from UE:%02x:%02x:%02x:%02x:%02x:%02x at AP:%s\n",bm_source_addr[0],
                bm_source_addr[1],bm_source_addr[2],bm_source_addr[3],bm_source_addr[4],
                bm_source_addr[5],bm_bssid);
               
        }
        //printf("************* INSIDE NEW CODE BLOCK *************\n");
        //printf("Copied to local array: Ref to Original Copy [%s] Local Copy [%s]\n",bssid,bm_bssid);

	//CWLog("**********************xml Parse File");
        doc = xmlParseFile(DOCNAME);

        if (doc == NULL) {
                fprintf(stderr, "Document not parsed successfully. \n");
                return (0);
        }
	
	//CWLog("*********************Get Root");
        cur = xmlDocGetRootElement(doc);

        if (cur == NULL) {
                fprintf(stderr, "empty document\n");
                xmlFreeDoc(doc);
                return (0);
        }

	//CWLog("************************String compare");
        if (xmlStrcmp(cur->name, (const xmlChar *)"init")) {
                fprintf(stderr, "document of the wrong type, root node != config");
                cur = cur->xmlChildrenNode;
                xmlFreeDoc(doc);
                return (0);
        }
	CWLog("************************Parse init call");
        parse_init(doc, cur);
	
	CWLog("************************Save file");
	//xmlSaveFile("buffer.xml", cur);
	xmlSaveFormatFileEnc("buffer.xml", doc, "UTF-8", 1);
	
        //printf("\n\n\n\n");

        return (0);
}
