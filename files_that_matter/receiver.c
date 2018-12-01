/* This will run on the reveiving server. It will take in the packets sent
*  by the queues. It will store each packet in a vector and, after it has
*  received the last packet, it will calculate the following:
*    average queue length
*    average time in queues
*    number of dropped packets
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>

int main()
{

  char str[25];
  int listen_fd, comm_fd;
  char end_string[25];
  for (i = 0; i < 25; i++) {
    end_string[i] = 'x';
  }

  struct sockaddr_in servaddr;

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  bzero( &servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(10010);

  bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  listen(listen_fd, 10);

  comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

  while(1)
  {
    bzero( str, 25);

    read(comm_fd,str,25);
    if (end_string == str) {
      printf("Final packet received\n")
      continue;
    }

    printf("Recieved new packet\n");
  }

  close(listen_fd);

}

//Calculates the average length of time the packets were in the queue. 
double avgWTime()
{
    double timeDiff;
    int totalPacks;
    double time;
    double timeAvg = 0;
    
    timeAvg = time/totalPacks;
    return timeAvg;
}

//Calculates the average length of the queue
double avgQLen()
{
    double sizeofQ;
    double avgLen = 0;
    double arrivalTime;
    double serviceTime;
    double roe = arrivalTime/serviceTime;
    
    avgLength = (roe/(1 - roe));
    return avgLen; 
}

//Calculate the probability a packet being dropped during the process
double blockProb()
{
    int totalPacks;
    int dropPacks;
    double packProb;
    
    packpProb = dropPacks/totalPacks;
    return packProb;
}
