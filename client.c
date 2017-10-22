#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <poll.h>
#include <time.h>


void error(const char *msg)
{
    perror(msg);
    exit(0);
}



// www.stackoverflow.com/questions/846006/how-do-i-concatenate-two-strings-in-c
char* concat(const char *s1, const char *s2){
char *result = malloc(strlen(s1)+strlen(s2)+2); //+1 for null terminator +2 for :
strcpy(result, s1);
strcat(result, ":");
strcat(result, s2);
return result;
}

//http://www.geeksforgeeks.org/time-delay-c/
void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds)
;
}


int main(int argc, char *argv[])
{
    int sockfd, portno, n, event;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct pollfd fds[0];
    char buffer[256];
    char abuffer[256];

    char *cmessageNO = "0";
    int messageNO = atoi(cmessageNO);
    char *cAck = "0";
    int Ack = atoi(cAck);
    int tempmsgNO = 0;
    char *header;
    char *buff;
    const char buff_Ack[6] = "Ack\n";
    //bool values
    int discard = 0;
    int Ackbool = 0;
    int repeating = 1;

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


//http://beej.us/guide/bgnet/output/html/multipage/pollman.html
fds[0].fd = sockfd;
fds[0].events = POLLIN;

fds[1].fd = 0;
fds[1].events = POLLIN;


while(1)
{
	event = poll(fds, 2, -1);

	// message recieving side
	if (fds[0].revents & POLLIN)
	{
		bzero(buffer,256);
		bzero(buff,strlen(buff));
		read(sockfd,buffer,256);

		//if message recieved is an Ack, set Ackbool to true
		if (buffer[0] == 'A' && buffer[1] == 'c' && buffer[2] == 'k'){Ackbool = 1;}
		
		//if buffer is not null and message is not an Ack
		if(buffer[0] != '\0' && Ackbool == 0){ 
		tempmsgNO = messageNO;

		//unpack message
		cmessageNO = strtok(buffer,":");
		cAck = strtok(NULL,":");
		buff = strtok(NULL,"\0");

		//update int value with recieved char values
		messageNO = atoi(cmessageNO);
		Ack = atoi(cAck);

		//if message is a duplicate, set discard to true
		if(tempmsgNO != messageNO-1){discard = 1;}
			}

		//If Ack is received and passes as valid, update Ack count
		if(Ackbool == 1 && messageNO-1 == Ack){Ack++;}

		// If message is not an Ack and it is the next message in sequence, reply with an Ack.
		if(Ackbool == 0 && messageNO-1 == Ack){
			Ack = messageNO;
			send(sockfd, buff_Ack, strlen(buff_Ack),0);
				}

		// If the message passes all tests print it to file
		if(messageNO == Ack && Ackbool == 0 && discard == 0){
		 fprintf(stdout,"%s", buff);
			fflush(stdout);
		}
			//reset bool values and clear buffers
			Ackbool = 0;
			discard = 0;
			bzero(buffer,256);
			bzero(buff,strlen(buff));

			//exit reciever
		}
		
		
		
	

// message sending side
	if (fds[1].revents & POLLIN)
	{
		bzero(buffer, 256);
		bzero(buff,strlen(buff));
		fgets(buffer, 255, stdin);
		
		
	if(buffer[0] != '\0'){ //stops segmentation error when using input command

		messageNO++;
		
		//store int values as char 
		cAck = malloc(16);
		cmessageNO = malloc(16);
		snprintf(cAck, 16, "%d", Ack);
		snprintf(cmessageNO, 16, "%d", messageNO);

		//Pack message
 		header = concat(cmessageNO,cAck); 
		buff = concat(header,buffer);

		//wait for ack before exiting loop 
 	while(repeating == 1){

		write(sockfd, buff, strlen(buff));
		//delay to stop messages being sent too quickly and causing read errors
		delay(25);

		//use seperate buffer 'abuffer' to look for ack otherwise code just gets stuck in loop
		bzero(abuffer,256);
		recv(sockfd,abuffer,256,MSG_DONTWAIT);
		if (abuffer[0] == 'A' && abuffer[1] == 'c' && abuffer[2] == 'k'){Ackbool = 1;repeating = 0;}
		//update ack if ack is recieved
		if(Ackbool == 1 && messageNO-1 == Ack){Ack++;}
			}

		//reset bool values
		Ackbool = 0;
		repeating = 1;
		
		}
	//exit sender side
	}

}

//couldnt shouldnt get here 
    close(sockfd);
    return 0;
}




