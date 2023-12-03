#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define DEFAULT_PORT                (1999)
#define SERV_ADDR_MAX_LEN           (16)
#define DEFAULT_SERVER_ADDRESS      "127.0.0.1"
#define MAX_DATA_RECV               (100)

#define CONNECTION_ERROR  (1)
#define OPTION_ERROR      (-1)
#define SUCCES            (0)

#define DISPLAY(...)      {printf("[%s:%s:%d]", __FILE__, __func__, __LINE__);printf(__VA_ARGS__);}

int main(int argc, char ** argv)
{

  int  port=DEFAULT_PORT, sock_fd, option='?';
  char str_serv_address[SERV_ADDR_MAX_LEN];
  snprintf(str_serv_address,strlen(DEFAULT_SERVER_ADDRESS)+1,DEFAULT_SERVER_ADDRESS);

  while ((option = getopt(argc, argv, ":p:a:h"))!=-1)
  {
    switch (option)
    {
      case 'a':
        snprintf(str_serv_address,strlen(optarg)+1,"%s",optarg);
        break;
      case 'p':
        port = atoi(optarg);
        break;
      case 'h':
        printf("%s [-p <port> | -h | -a <server address>]\n", argv[0]);
        return SUCCES;
      case '?':
        printf("wrong options\n");
      default :
        printf("%s [-p <port> | -h | -a <server address>]\n", argv[0]);
        return OPTION_ERROR;
    }
  }
  
  if((sock_fd = socket(AF_INET,SOCK_STREAM, 0))==-1)
  {
    DISPLAY("error : socket() %d\n",errno)
    return CONNECTION_ERROR;
  }
  DISPLAY("Create socket: OK\n")

  struct sockaddr_in server_address={AF_INET,  htons(port)};
  inet_pton(AF_INET, str_serv_address, &server_address.sin_addr);
  memset(&(server_address.sin_zero), 0, 8);
  if (connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
  {
    DISPLAY("error : connect() %d\n",errno)
    return CONNECTION_ERROR;
  }
  DISPLAY("Connect: OK\n")

  int num_bytes;
  char data_recv[MAX_DATA_RECV];
  while(num_bytes = recv(sock_fd, data_recv, MAX_DATA_RECV-1, 0))
  {
    if (num_bytes == -1)
    {
      DISPLAY("error : recv() %d\n",errno)
      break;
    }

    if(!num_bytes)
    {
      DISPLAY("Server closed the connection\n")
      break;
    }

    data_recv[num_bytes] = '\0';
    DISPLAY("Client received: %s\n", data_recv)
  }
  
  close(sock_fd);
  DISPLAY("Socket closed\n")

  return SUCCES;
}
