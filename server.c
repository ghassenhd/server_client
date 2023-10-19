#include <stdio.h>
#include <unistd.h>  
#include <stdlib.h>

#define DEFAULT_PORT    (1999)
#define MAX_NUM_CLIENTS (0)
#define OPTION_ERROR    (-1)
#define SUCCES          (0)

int main(int argc, char ** argv)
{
  int port=DEFAULT_PORT, num_clients=MAX_NUM_CLIENTS, option='?';

  while ((option = getopt(argc, argv, ":p:hn:"))!=-1)
  {
    switch (option)
    {
      case 'p':
        port = atoi(optarg);
        break;
      case 'n':
        num_clients = atoi(optarg);
        break;
      case 'h':
        printf("%s [-p <port> | -h | -n <max number of clients>]\n", argv[0]);
        return SUCCES;
      case '?':
        printf("wrong options\n");
      default :
        printf("%s [-p <port> | -h | -n <max number of clients>]\n", argv[0]);
        return OPTION_ERROR;
    }
  }


  return SUCCES;
}

