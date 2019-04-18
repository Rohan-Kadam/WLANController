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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv){
    
    int fd_downfifo, fd_upfifo, ret, ap_num;
    volatile int itr;
    char pathname_downfifo[50];
    char pathname_upfifo[50];
    pid_t childPID;
    char *msg = "Demo Message from Sec-Ctrller";
    char execl_str[100];
    char buffer[100];

    ap_num = atoi(argv[1]);
    printf("Number of Access Points to control = %d\n",ap_num);
    

    for(itr = ap_num ; itr > 0 ; --itr)
    {
        //Create Downlink FIFO
        sprintf(pathname_downfifo,"/tmp/downfifo%d",itr);

        if((mkfifo(pathname_downfifo, S_IRWXU | S_IRWXG))<0){
            printf("Could not create named pipe %s.\n", pathname_downfifo);
            printf("%s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        //Create Uplink FIFO
        sprintf(pathname_upfifo,"/tmp/upfifo%d",itr);

        if((mkfifo(pathname_upfifo, S_IRWXU | S_IRWXG))<0){
            printf("Could not create named pipe %s.\n", pathname_upfifo);
            printf("%s.\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    

        if((childPID = fork())<0){
            printf("Error forking process in %u.\n", getpid());
            exit(EXIT_FAILURE);
        }

    
    
        //in the child process --------------------------
        if(!childPID){

        char buffer[256];
        printf("In the child process %u.\n", getpid());

        //Assemble execl string here
        sprintf(execl_str,"/usr/local/bin/netopeer-cli /tmp/downfifo%d /tmp/upfifo%d",itr,itr);
        ret = execl("/usr/bin/gnome-terminal", "gnome-terminal","-e",execl_str,
                      NULL);
        if(ret<0) 
        { 
        	perror("error in execl1"); exit(2); 
        }
        exit(0);

        }
    }
    //in the parent process ----------------------------
    if(childPID){
        char buffer[256];
        
        printf("In the parent process %u.\n", getpid());
        
        printf("%s",pathname_downfifo);

        printf("\n\n================== Write 1 ==================\n\n");
        if((fd_downfifo = open(pathname_downfifo, O_WRONLY))<0)
        {
            printf("Error opening DOWN FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } 
        else 
        {
            printf("DOWN FIFO opened in %u.\n", getpid());
            write(fd_downfifo, msg, strlen(msg));
        }
        
        printf("%s",pathname_upfifo);

        printf("\n\n================== Read 1 ==================\n\n");
        if((fd_upfifo = open(pathname_upfifo, O_RDONLY))<0){
            printf("Error opening UP FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } else {
            printf("UP FIFO opened in %u.\n", getpid());
            read(fd_upfifo, buffer, sizeof(buffer));
            printf("READ In process %u, read: %s.\n", getpid(), buffer);
        }

        itr = strlen(pathname_downfifo);
        pathname_downfifo[itr -1] = '2';
        printf("%s",pathname_downfifo);
        
        printf("\n\n================== Write 2 ==================\n\n");
        if((fd_downfifo = open(pathname_downfifo, O_WRONLY))<0)
        {
            printf("Error opening DOWN FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } 
        else 
        {
            printf("DOWN FIFO opened in %u.\n", getpid());
            write(fd_downfifo, msg, strlen(msg));
        }

        itr = strlen(pathname_upfifo);
        pathname_upfifo[itr -1] = '2';
        printf("%s",pathname_upfifo);

        printf("\n\n================== Read 2 ==================\n\n");        
        if((fd_upfifo = open(pathname_upfifo, O_RDONLY))<0){
            printf("Error opening UP FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } else {
            printf("UP FIFO opened in %u.\n", getpid());
            read(fd_upfifo, buffer, sizeof(buffer));
            printf("READ In process %u, read: %s.\n", getpid(), buffer);
        }
        
		printf("Exiting process %u.\n", getpid());
        

		//select polling here
		while(1);

        if((unlink(pathname_upfifo))<0){
            printf("Error erasing %s.\n", pathname_upfifo);
        } else {
            printf("FIFO '%s' erased.\n", pathname_upfifo);
        }

        if((unlink(pathname_downfifo))<0){
            printf("Error erasing %s.\n", pathname_downfifo);
        } else {
            printf("FIFO '%s' erased.\n", pathname_downfifo);
        }

        printf("Exiting process %u.\n rlen = %d\n", getpid(), strlen(buffer));
    }
    
    return 0;
    
}
