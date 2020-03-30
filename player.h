#include "potato.h"

typedef struct player{
  int ID;
  int playsNum;
  potato_t Potato;
  const char* port;
  const char* host;
  // player info as server
  char server_port[12];
  struct addrinfo* local_host_info_list;
  int listen_fd;
  int server_with_right_fd;
  
  // player info as client
  struct addrinfo* master_host_info_list;
  int master_conn_fd; // with ring master

  int client_with_left_fd;
  
  char leftPort[12];
  
  char rightPort[12];
  
  
}player_t;
