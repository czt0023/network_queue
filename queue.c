/* This holds the queue for the project. Two of these will exist at any
*  given time. This must be able to return the length of its queue, in order
*  for load.c to determine where it should load packets. The queues must
*  mark the packets with a time stamp when they enter into the queue, as well
*  as a seperate timestamp when they leave the queue and are in transit to
*  the recieving server. They must also determine the queue length of this queue
*  at the time of sending the packet, and add that information to the sent
*  packet. They must then transmit the packets to the server. */


struct Element {
  char current_element = [25];
  Element next_element;
};

struct Queue {
  int queue_len;
  queue_element get_queue;
};
