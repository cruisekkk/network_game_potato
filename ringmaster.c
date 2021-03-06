#include "ringmaster.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/times.h>

// receive the user input
// set the basic information in the game:
// 1: the port number of ringmaster server
// 2: the number of players
// 3: the number of hops
void initGameCtrler(ringmaster_t* Game, char** input){
  Game->port = input[1];
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
}

// The initialized output for ringmaster
void initOutput(ringmaster_t* Game){
  printf("Potato Ringmaster\n");
  printf("Players = %d\n", Game->numPlayers);
  printf("Hops = %d\n", Game->numHops);
}

// This function set up the connections between players and master
// by using TCP funcitons
// Inside the function, there is a syncronization technique to keep the communication smooth
void SetUpServer(ringmaster_t* Game){
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = NULL;
  // clear the host info
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;
  status = getaddrinfo(hostname, Game->port, &host_info, &host_info_list);
  if (status != 0) {
    printf("Error: cannot get address info for host\n");
    if (hostname != NULL){
      printf("  (%s , %s)\n", hostname, Game->port);
    }
    exit(1);
  }

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    printf("Error: cannot create socket");
    if (hostname != NULL){
    printf("  (%s, %s)\n", hostname, Game->port);
    }
    exit(1);
  }

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    printf("Error: cannot bind socket");
    if (hostname != NULL){
    printf("  (%s, %s)\n", hostname, Game->port);
    }
    exit(1);
  }

  status = listen(socket_fd, 100);
  if (status == -1) {
    printf("Error: cannot listen on socket"); 
    if (hostname != NULL){
    printf("  (%s, %s)\n", hostname, Game->port);
    }
    exit(1);
  }

  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_player_fds[Game->numPlayers];

  // this loop is for the initialization of player's id for all players
  // they do not know their id before this master informed them
  // using send funciton
  for (int id = 0; id < Game->numPlayers; ++id){
    client_player_fds[id] = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_player_fds[id] == -1) {
      printf("Error: cannot accept connection on socket\n");
      exit(1);
    }
    send(client_player_fds[id], &id, sizeof(int), 0);
    printf("Player %d is ready to play\n", id);
    Game->client_fds[id] = client_player_fds[id];
  }

  // this loop is to send all players to know the numbers of the player
  // therefore they could determine their neighbors
  for (int id = 0; id < Game->numPlayers; ++id){
    sleep(1);
    send(client_player_fds[id], &(Game->numPlayers), sizeof(int), 0);
  }

  // syncronization IO
  for (int id = 0; id < Game->numPlayers; ++id){
    sleep(1);
    send(client_player_fds[id], &(Game->numPlayers), sizeof(int), 0);
  }

  // store the file descriptor information in the ring object
  Game->host_info_list = host_info_list;
  Game->listen_fd = socket_fd;
  return;
}


// this function would wait for time when all players connect to their neighbors 
void waitForPlayers(ringmaster_t* Game){
  int sync = 0;
  for (int id = 0; id < Game->numPlayers; ++id){
    recv(Game->client_fds[id], &sync, sizeof(int), 0);
  }
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
  // rich the information in the object ringmaster
  ringmaster_t GameCtrler;
  initGameCtrler(&GameCtrler,argv);
  initOutput(&GameCtrler);
  
  // set up the connections between players and master
  // collect the listen fd and clients' fd in the setting phase
  // send the id and total num to players
  SetUpServer(&GameCtrler);
  
  // wait for the connection among players
  waitForPlayers(&GameCtrler);

  printf("Ready to start game\n");
  // send potato
  // find the first player to send 
  int init_player_id = 0;
  srand((unsigned int)time(NULL) + init_player_id);
  init_player_id = rand() % (GameCtrler.numPlayers);
  GameCtrler.Potato.ID[0] = init_player_id;
  //printf("the init id is %d\n", GameCtrler.Potato.ID[0]);
  GameCtrler.Potato.hops = GameCtrler.numHops;

  // send the first players information and leave players to determine
  for (int i = 0; i < GameCtrler.numPlayers; ++i){
    send(GameCtrler.client_fds[i], &(GameCtrler.Potato.ID), sizeof(GameCtrler.Potato.ID),0);
    sleep(0.5);
    send(GameCtrler.client_fds[i], &(GameCtrler.numHops), sizeof(int),0);
  }

  // receive the potato trace
  // when any player send bit to the ringmaster
  // select funciton would notice ring to IO
  // read the trace using buffer int[1024]
  fd_set read_fds;
  int fdmax = 0;
  
  FD_ZERO(&read_fds);
  for(int i = 0; i < GameCtrler.numPlayers; i++){
    FD_SET(GameCtrler.client_fds[i], &read_fds);
    if(GameCtrler.client_fds[i] > fdmax)
      fdmax = GameCtrler.client_fds[i];
  }
  
  if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
    printf("Error on select funciton\n");
    return -1;
  }

  int trace[1024];
  for(int i = 0; i < GameCtrler.numPlayers; i++){
    if(FD_ISSET(GameCtrler.client_fds[i], &read_fds)){
      recv(GameCtrler.client_fds[i], &trace, sizeof(int[1024]), 0);
    }
  } 

  // display the trace of the potato
  printf("Trace of potato:\n");
  for (int i = 0; i < GameCtrler.numHops-1; i++){
    printf("<%d>,", trace[i]);
  }
  printf("<%d>\n", trace[GameCtrler.numHops-1]);

  // tell all of the players game is over
  for (int i = 0; i < GameCtrler.numPlayers; ++i){
    send(GameCtrler.client_fds[i], &(GameCtrler.Potato.hops), sizeof(int),0);
  }
  
  freeaddrinfo(GameCtrler.host_info_list);
  close(GameCtrler.listen_fd);
  return EXIT_SUCCESS;
}



