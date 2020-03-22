#include "ringmaster.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// receive the user input
// set the basic information in the game:
// 1: the port number of ringmaster server
// 2: the number of players
// 3: the number of hops
void initGameCtrler(ringmaster_t Game, char** input){
  Game.portNum = atoi(input[1]);
  Game.numPlayers = atoi(input[2]);
  Game.numHops = atoi(input[3]);
  if (Game.numPlayers < 1){
    fprintf(stderr, "invalid arguments\n");
    printf("the number of players should greater than 0\n");
    exit(1);
  }
  if (Game.numHops < 1){
    fprintf(stderr, "invalid arguments\n");
    printf("the number of hops should greater than 0\n");
    exit(1);
  }
  
  printf("Successful init Gamectrler\n");
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
  initGameCtrler(GameCtrler,argv);

  return EXIT_SUCCESS;
}



