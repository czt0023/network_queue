
#include<string.h>

int main()
{

  char str[25];
  int listen_fd, comm_fd;
  int i = 0;

  struct sockaddr_in servaddr;

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  bzero( &servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(10010);

  bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  listen(listen_fd, 10);

  comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
  int packets_received;

  while (1) {
    bzero( str, 25);

    read(comm_fd,str,25);
    if ('x' == str[0]) {
      printf("Shutdown packet received\n");
      break;
    }

    printf("Recieved new packet\n");
    printf("%s\n", str);
    packets_received++;
  }


  close(listen_fd);
  printf("%d packets received", packets_received);

}
