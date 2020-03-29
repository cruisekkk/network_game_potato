#include "potato.h"

typedef struct player{
  int ID;
  potato_t Potato;
  const char* port;
  const char* host;
  // player info as server
  struct addrinfo* local_host_info_list;
  int listen_fd;
  
  // player info as client
  struct addrinfo* master_host_info_list;
  int master_conn_fd; // with ring master

  
  
  const char* leftPort;
  const char* leftHost;
  
  
  const char* rightPort;
  const char* rightHost;
  
  
}player_t;
