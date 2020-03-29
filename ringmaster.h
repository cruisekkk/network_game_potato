#include "potato.h"
#define MAX_FDS 1024
#include <unistd.h>
typedef struct ringmaster{
  potato_t Potato;
  const char* port;
  int numPlayers;
  int numHops;
  int client_fds[MAX_FDS];
  struct addrinfo* host_info_list;
  int listen_fd;
  
}ringmaster_t;
