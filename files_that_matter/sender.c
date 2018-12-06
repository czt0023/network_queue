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
#include <math.h>
#include <inttypes.h>

#define PORT "10010"

// IP address of server
#define ADDRESS "131.204.2.9"

/* Defines the number of packets loaded at a time. Change this to
*  alter the arrival rate and test performance metrics */
const int ARRIVAL_RATE = 10;

/* Defines the number of packets that exit each queue. Packets
*  leave the queue after ARRIVAL_RATE number of packets have been loaded,
*  and SERVICE_RATE number of packets is sent from each queue with each
* send process.
* NEEDS TO BE MORE THAN DOUBLE THE ARRIVAL_RATE */
const int SERVICE_RATE = 1;

/* Defines the type of queueing
*    0: Minimum queue
*    1: Random queue */
const int QUEUE_METHOD = 1;


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

int sender(char output) {

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
    outbuf[0] = output;

    if ((numbytes = send(sockfd, outbuf, 25, 0)) == -1) {
      perror("Line 107");
      exit(1);

      printf("Client: sent to server");
    }

    printf("%s\n", outbuf);
  }

  /* The main function will create two queues, queue_1 and queue_2.
  *  The main function will
  *  then add new packets to each queue, based on either a minimum queue or RNG
  *  queueing system. the packets loaded must contain an initial time of arrival
  *  into the queue */

  int main() {

    // Two queues
    struct timespec queue_1[10000];
    struct timespec queue_2[10000];

    // Holds the length of each queue (or the number of values in them)
    int queue_1_len = 0;
    int queue_2_len = 0;

    // Holds teh current position in the queue
    int queue_1_pos = 0;
    int queue_2_pos = 0;

    // Hold the array length values for each sent packet
    int queue_1_len_store[10000];
    int queue_2_len_store[10000];

    // Stores the delta of time_in/time_out
    long time_diff_1[10000];
    long time_diff_2[10000];


    /* queue_method denotes the method taken when laoding the queue:
    *	 0 = minimum queue
    *  1 = random queue */
    int rand_queue = 0;

    srand(time(0));
    int i = 0;
    int packet_count;
    while (packet_count < 10000) {

      int j = 0;
      for (j; j < ARRIVAL_RATE; j++) {

        if (packet_count == 10000) { break;}

        struct timespec start_time;
        clock_gettime(CLOCK_REALTIME, &start_time);
        switch (QUEUE_METHOD) {
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
        packet_count++;
      }

      int k = 0;
      struct timespec final_time;
      // Simulate queues sending packets
      for (k; k < SERVICE_RATE; k++) {
        //sender('a');
        clock_gettime(CLOCK_REALTIME, &final_time);
        time_diff_1[queue_1_pos] = ((final_time.tv_sec) - (queue_1[queue_1_pos].tv_sec)) * 1000000000
          + ((final_time.tv_nsec) - (queue_1[queue_1_pos].tv_nsec));
        queue_1_len_store[queue_1_pos] = queue_1_len;
        queue_1_pos++;
        queue_1_len--;

        //sender('a');
        clock_gettime(CLOCK_REALTIME, &final_time);
        time_diff_2[queue_2_pos] = ((final_time.tv_sec) - (queue_2[queue_2_pos].tv_sec)) * 1000000000
          + ((final_time.tv_nsec) - (queue_1[queue_1_pos].tv_nsec));
        queue_2_len_store[queue_2_pos] = queue_2_len;
        queue_2_pos++;
        queue_1_len--;
      }
    }
    //sender('x');

    long sum = 0;
    int queue_len_sum = 0;
    for (i = 0; i < queue_1_pos; i++) {
      sum += time_diff_1[i];
    }
    printf("%lu Is the queue_1 average time (in nano seconds)\n", sum / queue_1_pos);

    for (i = 0; i < queue_1_pos; i++) {
      queue_len_sum += queue_1_len_store[i];
    }
    printf("%d Is the average queue length for queue_1\n\n", queue_len_sum / queue_1_pos);

    sum = 0;
    for (i = 0; i < queue_2_pos; i++) {
      sum += time_diff_2[i];
    }
    printf("%lu Is the queue_2 average time (in nano seconds)\n", sum / queue_2_pos);

    queue_len_sum = 0;
    for (i = 0; i < queue_2_pos; i++) {
      queue_len_sum += queue_2_len_store[i];
    }
    printf("%d Is the average queue length for queue_2\n", queue_len_sum / queue_2_pos);


    return 0;
  }
