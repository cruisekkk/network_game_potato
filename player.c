#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include "player.h"
#include <time.h>
#include <sys/times.h>

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
  printf("the first recv\n");
  int num = 0;
  // get the toal num of the players
  sleep(1);
  recv(socket_fd, &num, sizeof(int), 0);
  printf("the second recv\n");
  player->playsNum = num;
  printf("%d\n", num);
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
  for (int i = 0; i< 12; i++){
    player->server_port[i] = port[i];
  }
  
  if (player->ID == 0){
    sprintf(player->leftPort, "%d", (int)(atoi(player->port ) + player->playsNum));
  }
  else{
    sprintf(player->leftPort, "%d", (int)(atoi(player->port ) + player->ID));
  }
  
  if (player->ID == player->playsNum -1){
    sprintf(player->rightPort, "%d", (int)(atoi(player->port ) + 1));
  }
  else{
    sprintf(player->rightPort, "%d", (int)(atoi(player->port ) + player->ID + 1));
  }
  
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
  //printf("%s", port);
  return;
}


// as a server, it should use the listen_fd to receive
// send to 
void WaitRight(player_t* player){
  // receive the left player's hostname and
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  //if (player->ID == 0){
  int Rightfd = accept(player->listen_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  
  if (Rightfd == -1) {
    printf("Error: cannot accept connection on socket\n");
    exit(1);
  }
  //recv(socket_fd, &id, sizeof(int), 0);
  printf("Player %d accept \n", player->ID);
  //}
  //printf("Server received: %s\n", buffer);
  player->server_with_right_fd = Rightfd;
}


void ConnectToLeft(player_t* player){
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = player->host;
    
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  printf("%s", player->leftPort);
  status = getaddrinfo(hostname, player->leftPort, &host_info, &host_info_list);
  if (status != 0) {
    printf("Error: cannot get address info for host\n");
    if (hostname != NULL){
      printf("  (%s, %s)\n", hostname, player->leftPort);
    }
    exit(1);
  }

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    printf("Error: cannot create socket");
    if (hostname != NULL){
      printf("  (%s, %s)\n", hostname, player->leftPort);
    }
    exit(1);
  } 
  
  printf("Connecting to %s on port %s ...", hostname, player->leftPort);
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    printf("Error: cannot connect to socket");
    if (hostname != NULL){
      printf("  (%s, %s)\n", hostname, player->leftPort);
    }
    exit(1);
  }
  
  player->client_with_left_fd = socket_fd;
}

void start_receive_send(player_t* player){
  // receive from the master
  int ID[1024];
  int hops = -1;
  recv(player->master_conn_fd, &ID, sizeof(int[1024]), 0);
  sleep(0.5);
  recv(player->master_conn_fd, &hops, sizeof(int), 0);
  for(int i = 0; i < hops; i++){
    player->Potato.ID[i] = ID[i];
  }
  player->hops = hops;
  player->Potato.hops = player->hops;
  //printf("the hops all are %d\n", player->hops);
  //printf("the starter's ID is %d\n", player->Potato.ID[0]);
  
  if(player->Potato.ID[0] != player->ID){
    return;
  }
  // receive and resend
  //printf("i am the starter!");
  
  //hops -1!
  player->Potato.hops--;
  if(player->Potato.hops == 0){
    printf("see here %d\n", player->Potato.ID[0]);
    send(player->master_conn_fd, &player->Potato.ID, sizeof(int[1024]), 0);
  }

  // send hop and ID sequence to another player
  //// sending
  int player_fds[2];
  player_fds[0] = player->server_with_right_fd;
  player_fds[1] = player->client_with_left_fd;
  
  int send_player_order = 0;
  srand((unsigned int)time(NULL) + send_player_order);
  send_player_order = rand() % (2);
  // to right
  if (send_player_order == 0){
    if(player->ID != player->playsNum - 1){
      player->Potato.ID[1] = player->ID + 1;
    }
    else{
      player->Potato.ID[1] = 0;
    }
  }
  // to left
  if (send_player_order == 1){
    if(player->ID != 0){
      player->Potato.ID[1] = player->ID - 1;
    }
    else{
      player->Potato.ID[1] = player->playsNum -1;
    }
  }
  
  send(player_fds[send_player_order], &(player->Potato.hops), sizeof(int), 0);
  send(player_fds[send_player_order], &(player->Potato.ID), sizeof(int[1024]), 0);
}


void wait_send(player_t* player){
  fd_set read_fds;
  int fdmax;
  if (player->server_with_right_fd > player->client_with_left_fd){
    fdmax = player->server_with_right_fd;
  }
  else{
    fdmax = player->client_with_left_fd;
  }
  
  int player_fds[2];
  player_fds[0] = player->server_with_right_fd;
  player_fds[1] = player->client_with_left_fd;
  FD_ZERO(&read_fds);
  for(int i = 0; i < 2; i++){
    FD_SET(player_fds[i], &read_fds);
    if(player_fds[i] > fdmax)
      fdmax = player_fds[i];
  }
  
  if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
    printf("Error on select funciton\n");
    return;
  }

  for(int i = 0; i < 2; i++){
    if(FD_ISSET(player_fds[i], &read_fds)){
      recv(player_fds[i], &(player->Potato.hops), sizeof(int), 0);
      recv(player_fds[i], &(player->Potato.ID), sizeof(int[1024]), 0);
    }
  }
  player->Potato.hops--;
  if(player->Potato.hops == 0){
    send(player->master_conn_fd, &player->Potato.ID, sizeof(int[1024]), 0);
  }
  printf("after select");
  
  //// sending 
  int send_player_order = 0;
  srand((unsigned int)time(NULL) + send_player_order);
  send_player_order = rand() % (2);

  // to right
  if (send_player_order == 0){
    if(player->ID != player->playsNum - 1){
      player->Potato.ID[player->hops - player->Potato.hops] = player->ID + 1;
    }
    else{
      player->Potato.ID[player->hops - player->Potato.hops] = 0;
    }
  }
  // to left
  if (send_player_order == 1){
    if(player->ID != 0){
      player->Potato.ID[player->hops - player->Potato.hops] = player->ID - 1;
    }
    else{
      player->Potato.ID[player->hops - player->Potato.hops] = player->playsNum -1;
    }
  }
  
  send(player_fds[send_player_order], &(player->Potato.hops), sizeof(int),0);
  send(player_fds[send_player_order], &(player->Potato.ID), sizeof(int[1024]),0);
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
  printf("%d have connect to master", player.ID);
  //sleep(5);
  SetUpServer(&player);
  
  //
  //sync
  int sync = 0;
  recv(player.master_conn_fd, &sync, sizeof(int), 0);
    
  // as a server 
  // player 0 wait
  if (player.ID == 0){
    WaitRight(&player);
    printf("0 wait finished !\n");
  }

  if(player.ID != 0 && player.ID != 1){
    int s = 0;
    WaitRight(&player);
    ConnectToLeft(&player);
    send(player.master_conn_fd, &s, sizeof(int), 0);
  }
  
  if (player.ID == 1){
    int s = 0;
    ConnectToLeft(&player);
    WaitRight(&player);
    send(player.master_conn_fd, &s, sizeof(int), 0);
  }

  if (player.ID == 0){
    int s = 0;
    ConnectToLeft(&player);
    send(player.master_conn_fd, &s, sizeof(int), 0);
   }
 
  printf("\n start the game\n");

  start_receive_send(&player);
  while(1){
    wait_send(&player);
    sleep(1);
  }
  
  freeaddrinfo(player.master_host_info_list);
  close(player.master_conn_fd);
}
