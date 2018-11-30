/* This will run on the reveiving server. It will take in the packets sent
*  by the queues. It will store each packet in a vector and, after it has
*  received the last packet, it will calculate the following:
*    average queue length
*    average time in queues
*    number of dropped packets
*/
