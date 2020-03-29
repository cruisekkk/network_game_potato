#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include "player.h"

void initPlayer(player_t* player, char** input){
  player->host = input[1];
  player->port = input[2];
  // lack port valid check
}

void ConnectToMaster(player_t* player){
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = player->host;
  const char* port = player->port;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    printf("Error: cannot get address info for host\n");
    if (hostname != NULL){
      printf("  (%s, %s)\n", hostname, port);
    }
    exit(1);
  }

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    printf("Error: cannot create socket");
    if (hostname != NULL){
      printf("  (%s, %s)\n", hostname, port);
    }
    exit(1);
  } 
  
  printf("Connecting to %s on port %s ...", hostname, port);
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    printf("Error: cannot connect to socket");
    if (hostname != NULL){
      printf("  (%s, %s)\n", hostname, port);
    }
    exit(1);
  }

  int id = -1;
  // take the player's id
  recv(socket_fd, &id, sizeof(int), 0);
  printf("my id is %d", id);
  player->ID = id;

  player->master_host_info_list = host_info_list;
  player->master_conn_fd = socket_fd;
  return;
}


void SetUpServer(player_t* player){
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  char port[12];  
  
  sprintf(port, "%d", (int)(atoi(player->port ) + player->ID + 1));
  //printf()  
  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;
  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    printf("Error: cannot get address info for host\n");
    if (hostname != NULL){
      printf("  (%s , %s)\n", hostname, port);
    }
    exit(1);
  }

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    printf("Error: cannot create socket");
    if (hostname != NULL){
    printf("  (%s, %s)\n", hostname, port);
    }
    exit(1);
  }

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    printf("Error: cannot bind socket");
    if (hostname != NULL){
    printf("  (%s, %s)\n", hostname, port);
    }
    exit(1);
  }

  status = listen(socket_fd, 100);
  if (status == -1) {
    printf("Error: cannot listen on socket"); 
    if (hostname != NULL){
    printf("  (%s, %s)\n", hostname, port);
    }
    exit(1);
  }

  player->local_host_info_list = host_info_list;
  player->listen_fd = socket_fd;
  printf("ffffkkk");
  return;
}






int main(int argc, char** argv){
  if (argc != 3){
    fprintf(stderr, "invalid arguments numbers\n");
    printf("need two arguments here:\nmachineName, portNum\n");
    return EXIT_FAILURE;
  }

  for (int j = 0 ; j < (int)strlen(argv[2]); j++){
    if (argv[2][j]>= '0' && argv[2][j] <= '9'){
      continue;
    }
    else{
      fprintf(stderr, "invalid portnum\n");
      printf("portnum should be an integer\n");
      exit(1);
    } 
  }
  
  player_t player;
  initPlayer(&player, argv);
  // the first connection
  ConnectToMaster(&player);
  //
  SetUpServer(&player);

  //ConnectToLeft(&player);



  
  freeaddrinfo(player.master_host_info_list);
  close(player.master_conn_fd);
}
