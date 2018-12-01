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

struct Element {
  char current_element = [25];
  Element next_element;
};

struct Queue {
  int queue_len;
  queue_element get_queue;
};

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
  return 0;
}
