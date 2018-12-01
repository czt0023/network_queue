#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "10010" // the port client will be connecting to 
#define MAXSENDSIZE 10
#define MAXRCVSIZE 16 // max number of bytes we can get at once 
#define MAXINPUTSIZE 24

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
	
	char inbuf[MAXINPUTSIZE];
	char outbuf[MAXSENDSIZE];
	char rcvbuf[MAXRCVSIZE];
	
	int counter;
	char firstNum[11];
	char secondNum[11];
	int foundOp = 0;
	
	char operand;
	unsigned int firstInt, secondInt;
	
	unsigned int rcvFirstInt, rcvSecondInt, rcvResult;
	char rcvOperand, flag;

    if (argc != 2) {
        fprintf(stderr,"Client: usage: client hostname \n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "Client: getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("Client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("Client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

	// promt user for input
	printf("Client: enter equation for evaluation (int<+-/x>int): ");
	fgets(inbuf, MAXINPUTSIZE, stdin);
	inbuf[strlen(inbuf)] = '\0';
	printf("Client: read %s",inbuf);
	
	// parse input
	int i = 0;
	int j = 0;
	for(counter = 0; counter < strlen(inbuf); counter++){
		if(inbuf[counter] == '-' || inbuf[counter] == '+' || inbuf[counter] == '/' || inbuf[counter] == 'x') {
			operand = inbuf[counter];
			foundOp = 1;
			continue;
		}
		if(foundOp) {
			secondNum[i] = inbuf[counter];
			i++;
			continue;
		}
		else {
			firstNum[j] = inbuf[counter];
			j++;
		}
	}
	
	firstNum[j] = '\0';
	secondNum[i] = '\0';
	
	// convert to int
	firstInt = atoi(firstNum);
	secondInt = atoi(secondNum);
	
	// pack message
	firstInt = htonl(firstInt);
	secondInt = htonl(secondInt);
	
	outbuf[0] = operand;
	
	memcpy(outbuf + 1, &firstInt, 4);
	memcpy(outbuf + 5, &secondInt, 4);
	
	// send message
	if ((numbytes = send(sockfd, outbuf, 9, 0)) == -1) {
        perror("Client: sendto");
        exit(1);
    }
	
	printf("Client: sent %d bytes to server \n", numbytes);
	
	// receive response
    if ((numbytes = recv(sockfd, rcvbuf, MAXRCVSIZE, 0)) == -1) {
        perror("Client: recv");
        exit(1);
    }
	
	// extract message
	rcvOperand = rcvbuf[0];
	memcpy(&rcvFirstInt, rcvbuf + 1, 4);
	memcpy(&rcvSecondInt, rcvbuf + 5, 4);
	memcpy(&rcvResult, rcvbuf + 9, 4);
	flag = rcvbuf[13];
	
	// Convert to host byte order
	rcvFirstInt = ntohl(rcvFirstInt);
	rcvSecondInt = ntohl(rcvSecondInt);
	rcvResult = ntohl(rcvResult);
	
	// Print result
	printf("Client: received %u %c %u = %u; flag: %d \n", rcvFirstInt, rcvOperand, rcvSecondInt, rcvResult, flag);

    close(sockfd);

    return 0;
}