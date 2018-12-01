/* This file is responsible for creating the packets and feeding them
*  into the queues. It must determine which queue it should be in, based
*  on either a minimum queue system or random queue system. It should be able
*  to switch between these methods easily (most likely using a flag) */

#include <stdio.h>
#include <time.h>

#define PACKET_SIZE = 100
#define TIMESTAMP_SIZE = 20

queue queue_1;
queue queue_2;

// Should continuously send packets to the queues
int send_to_queue(int num_packets) {
  return 0;
}
