#define MAX_EVENTS 5
#define READ_SIZE 10
#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <fcntl.h>
#include <sys/inotify.h> 

int main()
{
	int running = 1, event_count, i;
	size_t bytes_read;
	char read_buffer[READ_SIZE + 1];
	struct epoll_event event, events[MAX_EVENTS];
	int file_fd, inoti_fd, watch_fd;
	int epoll_fd = epoll_create1(0);
 	
	if(epoll_fd == -1)
	{
		fprintf(stderr, "Failed to create epoll file descriptor\n");
		return 1;
	}
 	
 	inoti_fd = inotify_init();
    if (inoti_fd < 0)
	{
		fprintf(stderr, "Cannot obtain an inotify instance\n");
		close(epoll_fd);
		return 1;
	}
    	

 	watch_fd = inotify_add_watch(inoti_fd, ".", IN_MODIFY|IN_CREATE|IN_DELETE);
    if (watch_fd < 0)
    {
		fprintf(stderr, "Cannot add inotify watch \n");
		close(epoll_fd);
		return 1;
	}	

 	//epoll won't work for normal files. Used for pipes and sockets.
 	//file_fd = fopen("/home/user/Desktop/Work/repo_iitb_non3gpp/WLANController/auto/netconf/MW-netopeer/controller/test/epoll", (char *)"w+" );
	//
	//if(file_fd == -1)
	//{
	//	fprintf(stderr, "Failed to open Test file descriptor\n");
	//	return 1;
	//}
	
	//Read-only and edge trigger flags activated
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = inoti_fd;
 
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, inoti_fd, &event))
	{
		fprintf(stderr, "Failed to add file descriptor to epoll\n");
		close(epoll_fd);
		return 1;
	}
 
	while(running)
	{
		printf("\nPolling for input...\n");
		event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);
		printf("%d ready events\n", event_count);
		for(i = 0; i < event_count; i++)
		{
			printf("Reading file descriptor '%d' -- ", events[i].data.fd);
			bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
			printf("%zd bytes read.\n", bytes_read);
			read_buffer[bytes_read] = '\0';
			printf("Read '%s'\n", read_buffer);

			//fflush((void *)&(events[i].data.fd));			
 
			if(!strncmp(read_buffer, "stop\n", 5))
				running = 0;
		}
	}
 
	if(close(epoll_fd))
	{
		fprintf(stderr, "Failed to close epoll file descriptor\n");
		return 1;
	}
	return 0;
}
