#include "potato.h"
#define MAX_FDS 1024
#include <unistd.h>
// This is a struct that store the necessary data in the game as a game controller
// this potato is the most important data, realize the trace funciton by tracking hops 
// client fd array is the array will "select" on
// listen fd is the fd which give the access to all connections with player
typedef struct ringmaster{
  potato_t Potato;
  const char* port;
  int numPlayers;
  int numHops;
  int client_fds[MAX_FDS];
  struct addrinfo* host_info_list;
  int listen_fd;
  
}ringmaster_t;
