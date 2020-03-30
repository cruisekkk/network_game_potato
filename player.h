#include "potato.h"

// This is a struct that store every player's data in the game
typedef struct player{
  // ID, playsNum would be informed by master
  int ID;
  int playsNum;
  // a temporary struct to store the potato data
  potato_t Potato;
  // also be informed by master
  int hops;
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
  // need differnt port to connect
  // a server can only be bind with one socket in a port in the same machine
  char leftPort[12];
  char rightPort[12];
  
}player_t;
