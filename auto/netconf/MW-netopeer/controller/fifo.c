#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(void){
    
    int fd;
    char *pathname = "/tmp/theFIFO";
    pid_t childPID;
    
    if((mkfifo(pathname, S_IRWXU | S_IRWXG))<0){
        printf("Could not create named pipe %s.\n", pathname);
        printf("%s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if((childPID = fork())<0){
        printf("Error forking process in %u.\n", getpid());
        exit(EXIT_FAILURE);
    }
    //in the child process --------------------------
    if(!childPID){
        char *msg = "Do you have stairs in your house?";
        printf("In the child process %u.\n", getpid());
        if((fd = open(pathname, O_WRONLY))<0){
            printf("Error opening FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } else {
            printf("FIFO opened in %u.\n", getpid());
            write(fd, msg, strlen(msg));
        }
	printf("FD = %d\n",fd);
        printf("Exiting process %u.\n slen = %d\n ", getpid(), strlen(msg));
    }
    //in the parent process
    if(childPID){
        char buffer[256];
        printf("In the parent process %u.\n", getpid());
        if((fd = open(pathname, O_RDONLY))<0){
            printf("Error opening FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } else {
            printf("FIFO opened in %u.\n", getpid());
            read(fd, buffer, sizeof(buffer));
            printf("READ In process %u, read: %s.\n", getpid(), buffer);
        }
	
	while(1);

        if((unlink(pathname))<0){
            printf("Error erasing %s.\n", pathname);
        } else {
            printf("FIFO '%s' erased.\n", pathname);
        }
        printf("Exiting process %u.\n rlen = %d\n", getpid(), strlen(buffer));
    }
    
    return 0;
    
}
