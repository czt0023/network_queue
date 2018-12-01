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

/* The queue structure will hold the data for the queues. It is implemented
*  using a linked list */

typedef struct Element {
  char current_element[25];
  Element next_element;
}Element_t;

typedef struct Queue {
  int queue_len;
  Element element_queue;
}Queue_t;

/* The sender function will conntinuously send packets to the server
*  from the queue. It must increment decrement the queue length, and
*  add the timestamp for fin_time and the queue length to the packet
*  before sending */

void sender(Queue *queue) {

}

/* The main function will create two queues, queue_1 and queue_2. These will
*  be put onto seperate threads and run concurrently. The main function will
*  then add new packets to each queue, based on either a minimum queue or RNG
*  queueing system. the packets loaded must contain an initial time of arrival
*  into the queue */

int main() {
  Queue queue_1;
  Queue queue_2;
  pthread thread_1;
  pthread thread_2;

  pthread_create(&thread_1, NULL, sender(queue_1), NULL);
  pthread_create(&thread_2, NULL, sender(queue_2), NULL);

  for (i = 0; i <= 10000; i++) {
    // add a new packet to a queue
  }
  return 0;
}
