#include "ringmaster.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

// receive the user input
// set the basic information in the game:
// 1: the port number of ringmaster server
// 2: the number of players
// 3: the number of hops
void initGameCtrler(ringmaster_t* Game, char** input){
  Game->portNum = atoi(input[1]);
  Game->numPlayers = atoi(input[2]);
  Game->numHops = atoi(input[3]);
  if (Game->numPlayers < 1){
    fprintf(stderr, "invalid arguments\n");
    printf("the number of players should greater than 0\n");
    exit(1);
  }
  if (Game->numHops < 1){
    fprintf(stderr, "invalid arguments\n");
    printf("the number of hops should greater than 0\n");
    exit(1);
  }
  
  printf("Successful init Gamectrler\n");
}


void SetUpServer(ringmaster_t* GameCtrler){
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  const char *port     = "4444";

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

  printf("Waiting for connection on player 0\n");
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;
  client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    printf("Error: cannot accept connection on socket\n");
    exit(1);
  } 

  char buffer[512];
  recv(client_connection_fd, buffer, 9, 0);
  buffer[9] = 0;

  printf("Server received: %s", buffer);

  freeaddrinfo(host_info_list);
  close(socket_fd);
  return;
}

void waitForPlayers(ringmaster_t* GameCtr){
  
  
}

int main(int argc, char** argv){
  if (argc != 4){
    fprintf(stderr, "invalid arguments numbers\n");
    printf("need three arguments here:\nportNum, numPlayers, numHops\n");
    return EXIT_FAILURE;
  }
  // check if there is any invalid input 
  for (int i = 1; i < 4; i++){
    for (int j = 0 ; j < (int)strlen(argv[i]); j++){
      if (argv[i][j]>= '0' && argv[i][j] <= '9'){
        continue;
      }
      else{
        fprintf(stderr, "invalid arguments\n");
        printf("all inputs should be integer\n");
        exit(1);
      }
    }
  }
  
  ringmaster_t GameCtrler;
  initGameCtrler(&GameCtrler,argv);
  SetUpServer(&GameCtrler);
  waitForPlayers(&GameCtrler);
  return EXIT_SUCCESS;
}



