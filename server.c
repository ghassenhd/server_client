#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#define MAX_NUM_CLIENTS   (1000)
#define DEFAULT_PORT      (1999)
#define BACKLOG           (512 ) //shoud not be bigger then the value 
                                 //in "/proc/sys/net/ipv4/tcp_max_syn_backlog".

#define OPTION_ERROR    (-1)
#define SUCCES          (0)

#define DISPLAY(...)      {printf("[%s:%s:%d]", __FILE__, __func__, __LINE__);printf(__VA_ARGS__);}

struct ClientData
{
    int client_fd;
    int connection_id;
    pthread_t  thread_id;
};

sem_t sema;
pthread_mutex_t lock;
int nb_clients=0;

void* handle_client(void* client_data)
{
  struct ClientData* data = (struct ClientData*)client_data;
  int connection_id = data->connection_id;
  int client_fd = data->client_fd;
  pthread_t  thread_id= data->thread_id;
  char message[1024];
  snprintf(message,1024,"Hello client %d",connection_id);
  if (send(client_fd,message,strlen(message), 0) == -1)
    DISPLAY("error : send() %d\n",errno) 

  pthread_mutex_lock(&lock);
  nb_clients--;
  pthread_mutex_unlock(&lock);
  close(client_fd);
  sem_post(&sema);
  free(data);
  return NULL;
}


int main(int argc, char ** argv)
{
  int port=DEFAULT_PORT, max_clients=MAX_NUM_CLIENTS, option='?';
  int sock_fd, new_fd, sin_size;
  struct sockaddr_in my_addr, their_addr;

  while ((option = getopt(argc, argv, ":p:hn:"))!=-1)
  {
    switch (option)
    {
      case 'p':
        port = atoi(optarg);
        break;
      case 'n':
        max_clients = atoi(optarg);
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

  if((sock_fd = socket(AF_INET,SOCK_STREAM, 0))==-1)
  {
    DISPLAY("error : socket() %d\n",errno)
    return -1;
  }
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), 0, 8);

  if (bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)))
  {
    DISPLAY("error : bind() %d\n",errno)
    return -1;
  }
  if (listen(sock_fd, BACKLOG))
  {
    DISPLAY("error : listen() %d\n",errno)
    return -1;
  }

  if(sem_init(&sema, 0, max_clients))
  {
    DISPLAY("error : sem_init() %d\n",errno)
    return -1;
  }
  if (pthread_mutex_init(&lock, NULL))
  {
    DISPLAY("error : pthread_mutex_init()\n")
    return -1;
  }

  int connection_id=0;
  struct ClientData * client_data=NULL;
  while (1)
  {
    if(sem_wait(&sema))
    {
      DISPLAY("error : sem_wait() %d\n",errno)
      continue;
    }  
    new_fd = accept(sock_fd, (struct sockaddr*)&their_addr, &sin_size);
    if (new_fd == -1)
    {
      DISPLAY("error : accept()\n")
      sem_post(&sema);
      continue;
    } 
    client_data = (struct ClientData*)malloc(sizeof(struct ClientData));
    client_data->client_fd=new_fd;
    client_data->connection_id=++connection_id;
    if(pthread_create(&(client_data->thread_id),NULL,handle_client,(void*)client_data))
    {
      DISPLAY("error : pthread_create()\n")
      sem_post(&sema);
      if (send(new_fd, "server couldn't create a thread for the client\n", 47, 0) == -1)
        DISPLAY("error : send()\n")
      close(new_fd);
      continue;
    }
    pthread_detach(client_data->thread_id);
    pthread_mutex_lock(&lock);
    nb_clients++;
    pthread_mutex_unlock(&lock);
  }
  pthread_mutex_destroy(&lock);
  close(sock_fd);
  sem_destroy(&sema);
  return SUCCES;
}

