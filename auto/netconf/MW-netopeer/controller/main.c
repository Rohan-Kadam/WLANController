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

int main(void){
    
    int fd, fd_downfifo, fd_upfifo, ret, flag=0;
    char *pathname_downfifo = "/tmp/downfifo";
    char *pathname_upfifo = "/tmp/upfifo";
    pid_t childPID;
    char *msg = "Demo Message from Sec-Ctrller";
    char buffer[100];

    //Create Downlink FIFO
    if((mkfifo(pathname_downfifo, S_IRWXU | S_IRWXG))<0){
        printf("Could not create named pipe %s.\n", pathname_downfifo);
        printf("%s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //Create Uplink FIFO
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
        
        //Execl here
        ret = execl("/usr/bin/gnome-terminal", "gnome-terminal","-e","/usr/local/bin/netopeer-cli /tmp/downfifo /tmp/upfifo",
                      NULL);
        if(ret<0) 
        { 
        	perror("error in execl1"); exit(2); 
        }
        /*
        if((fd = open(pathname, O_RDONLY | O_NONBLOCK))<0){
            printf("Error opening FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } else {
            printf("FIFO opened in %u.\n", getpid());
            read(fd, buffer, sizeof(buffer));
            printf("READ In process %u, read: %s.\n", getpid(), buffer);
        }
		printf("Exiting process %u.\n", getpid());
        */
        exit(0);

    }

    //in the parent process ----------------------------
    if(childPID){
        char buffer[256];
        
        printf("In the parent process %u.\n", getpid());
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
