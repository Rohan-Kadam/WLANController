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
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event

#define MAX_EVENTS 5
#define READ_SIZE 100

int CTRL2MW_fd;
char *pathname = "/tmp/CTRL2MW_FIFO";
    
void netopeer_init(void)
{
    //int MW2AP_fd;
    //char *pathname = "/tmp/MW2AP_FIFO";

    mkfifo(pathname, S_IRWXU | S_IRWXG);
    CTRL2MW_fd = open(pathname, O_RDONLY, O_NONBLOCK);

}

int main(int argc, char** argv){
    
    int fd_downfifo[10], fd_upfifo[10], ret, ap_num;
    volatile int itr;
    char pathname_downfifo[50];
    char pathname_upfifo[50];
    pid_t childPID;
    char *msg = "Demo Message from Sec-Ctrller";
    char execl_str[100];
    char buffer[100];

    int running = 1, event_count, i;
    size_t bytes_read;
    char read_buffer[READ_SIZE + 1];
    struct epoll_event event, events[MAX_EVENTS];
    int file_fd, inoti_fd, watch_fd, notif_fd;

    int epoll_fd;

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

        //Assembling execl string
        sprintf(execl_str,"/usr/local/bin/netopeer-cli /tmp/downfifo%d /tmp/upfifo%d",itr,itr);
        //block spawning of other child processes
        ret = execl("/usr/bin/gnome-terminal", "gnome-terminal","-e",execl_str,
                      NULL);
        //ret = execl(execl_str,
        //              NULL);
        if(ret<0) 
        { 
        	perror("error in execl1"); exit(2); 
        }
        exit(0);

        }
        
        //in the parent process ----------------------------
        if(childPID){

            //TODO:
            //Maintain list of all fifo descriptors and their paths 
            //in a array of character 

        }
    
    }
    //in the parent process ----------------------------
    if(childPID){
        char buffer[256];
        
        printf("In the parent process %u.\n", getpid());
    
        epoll_fd = epoll_create1(0);    
        if(epoll_fd == -1)
        {
            fprintf(stderr, "Failed to create epoll file descriptor\n");
            return 1;
        }

        printf("%s",pathname_downfifo);

        printf("\n\n================== Write 1 ==================\n\n");
        if((fd_downfifo[0] = open(pathname_downfifo, O_WRONLY , O_NONBLOCK))<0)
        {
            printf("Error opening DOWN FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } 
        else 
        {
            printf("DOWN FIFO opened in %u.\n", getpid());
            itr = write(fd_downfifo[0], msg, strlen(msg));
            printf("Bytes written in pipe 1 = %d, fd-down = %d\n",itr,fd_downfifo[0]);
            if(itr < 0)
            {
                perror("ERROR");
            }
        }
        
        printf("%s",pathname_upfifo);

        printf("\n\n================== Read 1 ==================\n\n");
        if((fd_upfifo[0] = open(pathname_upfifo, O_RDONLY))<0){
            printf("Error opening UP FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } 
        //else 
        //{
            printf("UP FIFO opened in %u.\n", getpid());
        //Change FD to array if enabled
        //    read(fd_upfifo, buffer, sizeof(buffer));
        //    printf("READ In process %u, read: %s.\n", getpid(), buffer);
        //}

        //Replacing index of fifo with 2
        itr = strlen(pathname_downfifo);
        pathname_downfifo[itr -1] = '2';
        printf("%s",pathname_downfifo);
        
        printf("\n\n================== Write 2 ==================\n\n");
        if((fd_downfifo[1] = open(pathname_downfifo, O_WRONLY , O_NONBLOCK))<0)
        {
            printf("Error opening DOWN FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } 
        else 
        {
            printf("DOWN FIFO opened in %u.\n", getpid());
            itr = write(fd_downfifo[1], msg, strlen(msg));
            printf("Bytes written in pipe 2 = %d\nfd-down = %d\n",itr,fd_downfifo[1]);
            if(itr < 0)
            {
                perror("ERROR");
            }
        }

        //Replacing index of fifo with 2
        itr = strlen(pathname_upfifo);
        pathname_upfifo[itr -1] = '2';
        printf("%s",pathname_upfifo);

        printf("\n\n================== Read 2 ==================\n\n");        
        if((fd_upfifo[1] = open(pathname_upfifo, O_RDONLY))<0){
            printf("Error opening UP FIFO in %u.\n", getpid());
            exit(EXIT_FAILURE);
        } 
        //else 
        //{
            printf("UP FIFO opened in %u.\n", getpid());
        //Change FD to array if enabled
        //    read(fd_upfifo, buffer, sizeof(buffer));
        //    printf("READ In process %u, read: %s.\n", getpid(), buffer);
        //}
        
		//printf("Exiting process %u.\n", getpid());
        
        printf("\n\n====================== READ FROM CONTROLLER PIPE ======================\n\n");        
        netopeer_init();


        event.events = EPOLLIN | EPOLLET;
        event.data.fd = fd_upfifo[0];
 
        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_upfifo[0], &event))
        {
            fprintf(stderr, "1 Failed to add file descriptor to epoll\n");
            close(epoll_fd);
            return 1;
        }

        event.events = EPOLLIN | EPOLLET;
        event.data.fd = fd_upfifo[1];
 
        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_upfifo[1], &event))
        {
            fprintf(stderr, "2 Failed to add file descriptor to epoll\n");
            close(epoll_fd);
            return 1;
        }
 
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = CTRL2MW_fd;

        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, CTRL2MW_fd, &event))
        {
            fprintf(stderr, "3 Failed to add file descriptor to epoll\n");
            close(epoll_fd);
            return 1;
        }

        printf("Descriptors:\nupfifo1 =%d upfifo1 =%d downfifo =%d\n",fd_upfifo[0],fd_upfifo[1],CTRL2MW_fd);


        while(running)
        {
            printf("\nPolling for input...\n");
            event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);
            printf("%d ready events\n", event_count);
            for(i = 0; i < event_count; i++)
            {
                printf("Inside epoll for loop,\nfd = %d\nfd-up0 = %d\nfd-up1 = %d\n",events[i].data.fd, fd_upfifo[0], fd_upfifo[1]);
                if(events[i].data.fd == fd_upfifo[0])
                {
                    printf("Reading file descriptor '%d' -- ", events[i].data.fd);
                    bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
                    printf("%zd bytes read.\n", bytes_read);
                    read_buffer[bytes_read] = '\0';
                    printf("READ: '%s'\n", read_buffer);

                    notif_fd = fopen("/etc/toCTRL.txt","ab");

                    if(notif_fd == NULL)
                    {
                        printf("fopen failed");
                    }

                    //TODO: Map MAC/IP to APID
                    //TODO: Modify string here (i.e. Add APID Mapping, remove tags,...)    
                    
                    //Update Notification to Controller file
                    fputs(read_buffer, notif_fd);
                    fclose(notif_fd);
                } 
            
                if(events[i].data.fd == fd_upfifo[1])
                {
                    printf("Reading file descriptor '%d' -- ", events[i].data.fd);
                    bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
                    printf("%zd bytes read.\n", bytes_read);
                    read_buffer[bytes_read] = '\0';
                    printf("READ: '%s'\n", read_buffer);

                    //Write to notification file here
                    notif_fd = fopen("/etc/toCTRL.txt","ab");

                    if(notif_fd == NULL)
                    {
                        printf("fopen failed");
                    }

                    //TODO: Map MAC/IP to APID
                    //TODO: Modify string here (i.e. Add APID Mapping, remove tags,...)    
                    
                    //Update Notification to Controller file
                    fputs(read_buffer, notif_fd);
                    fclose(notif_fd);

                } 
                
                if(events[i].data.fd == CTRL2MW_fd)
                {
                    printf("Reading file descriptor '%d' -- ", events[i].data.fd);
                    bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
                    printf("%zd bytes read.\n", bytes_read);
                    read_buffer[bytes_read] = '\0';
                    printf("READ: '%s'\n", read_buffer);

                    
                    //Write to downlink pipe.
                    printf("DOWN FIFO opened in %u.\n", getpid());
                    itr = write(fd_downfifo[0], read_buffer, strlen(read_buffer));
                    printf("Bytes written in pipe 1 = %d, fd-down = %d\n",itr,fd_downfifo[0]);
                    if(itr < 0)
                    {
                        perror("ERROR");
                    }

                    itr = write(fd_downfifo[1], read_buffer, strlen(read_buffer));
                    printf("Bytes written in pipe 1 = %d, fd-down = %d\n",itr,fd_downfifo[1]);
                    if(itr < 0)
                    {
                        perror("ERROR");
                    }

                }
                //if ((events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN)) && events[i].data.fd != fd_upfifo[0]) {
                //perror("epoll error");
                //close(events[i].data.fd);
                //}

                //fflush((void *)&(events[i].data.fd));         
 
                //if(!strncmp(read_buffer, "stop\n", 5))
                //    running = 0;
            }
        }
 
        if(close(epoll_fd))
        {
            fprintf(stderr, "Failed to close epoll file descriptor\n");
            return 1;
        }


        //while(1);

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
