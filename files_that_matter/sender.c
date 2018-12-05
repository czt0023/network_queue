#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/queue.h>

#define PORT "10010"

#define ADDRESS "131.204.2.9"

/* Defines the number of packets loaded at a time. Change this to
*  alter the arrival rate and test performance metrics */
const int ARRIVAL_RATE = 5;

/* Defines the number of packets that exit each queue. Packets
*  leave the queue after ARRIVAL_RATE number of packets have been loaded,
*  and SERVICE_RATE number of packets is sent from each queue with each
* send process. */
const int SERVICE_RATE = 1;
const int array_x = 10000;
const int array_y = 25;

/* The sender function will conntinuously send packets to the server
*  from the queue. It must increment decrement the queue length, and
*  add the timestamp for fin_time and the queue length to the packet
*  before sending */

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int sender(time_t *time_array) {

	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	char inbuf[25];
	char rcvbuf[25];

	int counter;
	char firstNum[11];
	char secondNum[11];
	int foundOp = 0;

	char operand;
	unsigned int firstInt, secondInt;

	unsigned int rcvFirstInt, rcvSecondInt, rcvResult;
	char rcvOperand, flag;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ADDRESS, PORT, &hints, &servinfo)) != 0) {
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

    char outbuf[25];
    outbuf[0] = 'x';

    if ((numbytes = send(sockfd, outbuf, 25, 0)) == -1) {
      perror("Line 107");
      exit(1);

      printf("Client: sent to server");
    }

    printf("%s\n", outbuf);


    return 0;
  }

  /* The main function will create two queues, queue_1 and queue_2.
  *  The main function will
  *  then add new packets to each queue, based on either a minimum queue or RNG
  *  queueing system. the packets loaded must contain an initial time of arrival
  *  into the queue */

  int main() {
    time_t queue_1[10000];
    time_t queue_2[10000];
    int queue_1_len = 0;
    int queue_2_len = 0;


    /* queue_method denotes the method taken when laoding the queue:
    *	 0 = minimum queue
    *  1 = random queue */
    int queue_method = 0;
    int rand_queue = 0;

    srand(time(0));
    int i = 0;
    for (i; i < 5; i++) {

      int j = 0;
      for (j; j < ARRIVAL_RATE; j++) {

        time_t start_time = time(NULL);
        switch (queue_method) {
          // Minimum queue model
          case 0:
          if (queue_1_len < queue_2_len) {
            queue_1[queue_1_len + 1] = start_time;
            queue_1_len++;
          }
          if (queue_1_len > queue_2_len) {
            queue_2[queue_2_len + 1] = start_time;
            queue_2_len++;
          }
          else {
            queue_1[queue_1_len + 1] = start_time;
            queue_1_len++;
          }
          break;

          // Random queue model
          case 1:
          rand_queue = rand() % 2;
          if (rand_queue == 0) {
            queue_1[queue_1_len + 1] = start_time;
            queue_1_len++;
          }
          else {
            queue_2[queue_2_len + 1] = start_time;
            queue_2_len++;
          }
          break;
        }
      }

      sender(queue_1);
      sender(queue_2);
    }

    return 0;
  }
