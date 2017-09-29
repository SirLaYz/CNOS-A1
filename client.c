#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <poll.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n, event;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct pollfd fds[0];
    char buffer[256];
    
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
/*
while(1){
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
}

*/




fds[0].fd = sockfd;
fds[0].events = POLLIN;

fds[1].fd = 0;
fds[1].events = POLLIN;

/* Break down the header+message string into characters, convert each character into binary and create a checksum for it. send the character+checksum binary wait for ack, send next character+checksum.
after each character+checksum is recieved and is uncorrupted convert it back into a character and add it to its proper buffer e.g header file buffer or message buffer starting with location [0]  
you will also need to check for duplication of messages and the order of each message*/

// 11111111111111111 17bits sequence number : 

// Data offset e.g headersize
// Sequence Number
// Acknowledgment Number
// TCP checksum
unsigned int messageNO = 0;
int ack;


while(1)
{
	
	event = poll(fds, 2, -1);

// message recieved side
	if (fds[0].revents & POLLIN)
	{
		bzero(buffer,256);
		n = read(sockfd, buffer, 256);
		messageNO++;

// Read Esc and the program will exit before printing the message
          if (buffer[0] == 'E' && buffer[1] == 's' && buffer[2] == 'c')
          {
           close(sockfd);
           return 0;
          }


		printf("%s", buffer);
    		
	}


// message sent side
	if (fds[1].revents & POLLIN)
	{
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		n = write(sockfd, buffer, strlen(buffer));
		messageNO++;
		
// Send Esc and the program will exit after sending the message		
          if (buffer[0] == 'E' && buffer[1] == 's' && buffer[2] == 'c')
          {
           close(sockfd);
           return 0;
          }
		
	}
}

//couldnt shouldnt get here 
    close(sockfd);
    return 0;
}


/*
fds[0].fd = sockfd;
fds[0].events = POLLIN;

fds[1].fd = 0;
fds[1].events = POLLIN;

while(1)
{
	event = poll(fds, 2, -1);


	if (fds[0].revents & POLLIN)
	{
		bzero(in_buffer,256);
		recv(sockfd, in_buffer, 256, 0);
		printf("%s", in_buffer);
	}

	if (fds[1].revents & POLLIN)
	{
		bzero(out_buffer, 256);
		fgets(out_buffer, 255, stdin);
		send(sockfd, out_buffer, strlen(out_buffer),0);
	}
}
*/



