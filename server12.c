#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "10010"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
#define MAXINSIZE 10
#define MAXOUTSIZE 16

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);
/Users/edwards56/Library/Containers/com.apple.mail/Data/Library/Mail Downloads/A0B56D33-33A8-42A3-AD41-40B66F114198/server12.c
    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
	
	char outbuf[MAXOUTSIZE];
	char inbuf[MAXINSIZE];
	int numbytes;
	unsigned int max_int = 4294967295;
	
	char flag = 1;
	unsigned int result, firstInt, secondInt;
	char operand;
	
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "Server: getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("Server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("Server: setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "Server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("Server: listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Server: sigaction");
        exit(1);
    }

    printf("Server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("Server: accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("Server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            if ((numbytes = recv(new_fd, inbuf, MAXINSIZE-1, 0)) == -1) {
				perror("Server: recv");
				exit(1);
			};
			
			printf("Server: received %d bytes\n", numbytes);
			
			// Extract message
			memcpy(outbuf, inbuf, 9);
			operand = inbuf[0];
			memcpy(&firstInt, inbuf + 1, 4);
			memcpy(&secondInt, inbuf + 5, 4);
			
			// Convert to host byte order
			firstInt = htonl(firstInt);
			secondInt = htonl(secondInt);
			
			printf("Server: received %u %c %u\n", firstInt, operand, secondInt);
			
			
			// perform calculation
			flag = 1;
			
			// Multiplication; checks for overflow
			if(operand == 'x' || operand == 'X') {
				if( secondInt > max_int/firstInt) {
					result = 0;
					flag = 2;
				}
				else { 
					result = firstInt*secondInt;
				}
			};
			
			// Subtraction; checks for negative result
			if(operand == '-') {
				if( secondInt > firstInt) {
					result = 0;
					flag = 2;
				}
				else {
					result = firstInt - secondInt;
				}
			};
			
			// Addition; checks for overflow
			if(operand == '+') {
				if( secondInt > max_int - firstInt) {
					result = 0;
					flag = 2;
				}
				else {
					result = firstInt + secondInt;
				}
			};
			
			// Division; checks for divide by zero
			if(operand == '/') {
				if( secondInt == 0) {
					result = 0;
					flag = 2;
				}
				else {
					result = firstInt/secondInt;
				}
			};
			
			
			printf("Server: calculated %u %c %u = %u; flag: %d\n", firstInt, operand, secondInt, result, flag);
			
			// Package into message
			result = htonl(result);
			memcpy(outbuf + 9, &result, 4);
			outbuf[13] = flag;
			
			// Send return message
			if ((numbytes = send(new_fd, outbuf, 14, 0)) == -1) {
				perror("talker: sendto");
				exit(1);
			}
			printf("Server: sent %d bytes\n", numbytes);
			
			// Done, so close connection
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}
