#include <stdio.h>
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
#include <pthread.h>

#define PORT_1 "10010"
#define PORT_2 "10011"

#define ADDRESS "131.204.2.9"

/* The queue structure will hold the data for the queues. It is implemented
*  using a linked list */

typedef struct Element {
  char current_element[25];
  struct Element * next_element;
}Element_t;

typedef struct Queue {
  int queue_len;
  Element_t element_queue;
}Queue_t;

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

void *sender(Queue_t *queue, int port) {

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

  if (port == 1) {
    if ((rv = getaddrinfo(ADDRESS, PORT_1, &hints, &servinfo)) != 0) {
      fprintf(stderr, "Client: getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }
  }
  if (port == 2) {
    if ((rv = getaddrinfo(ADDRESS, PORT_2, &hints, &servinfo)) != 0) {
      fprintf(stderr, "Client: getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }
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
    int i;
    for (i = 0; i < 25; i++) {
      outbuf[i] = 'x';
    }
    if ((numbytes = send(sockfd, outbuf, 25, 0)) == -1) {
      perror("Line 107");
      exit(1);

      printf("Client: sent to server");
    }



    return 0;
  }

  /* The main function will create two queues, queue_1 and queue_2. These will
  *  be put onto seperate threads and run concurrently. The main function will
  *  then add new packets to each queue, based on either a minimum queue or RNG
  *  queueing system. the packets loaded must contain an initial time of arrival
  *  into the queue */

  int main() {
    Queue_t queue_1;
    Queue_t queue_2;
    pthread_t thread_1;
    pthread_t thread_2;
    int port_1 = 1;
    int port_2 = 2;

    //   pthread_create(&thread_1, NULL, sender(&queue_1, port_1), NULL);
    //   pthread_create(&thread_2, NULL, sender(&queue_2, port_2), NULL);
    sender(&queue_1, port_1);

    for (int i = 0; i <= 10000; i++) {
      // add a new packet to a queue
    }
    return 0;
  }
