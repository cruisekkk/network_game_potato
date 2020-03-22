#include "potato.h"

typedef struct player{
  int ID;
  potato_t Potato;
  const char* portNum;
  const char* host;
  
  const char* leftPort;
  const char* leftHost;
  
  const char* rightPort;
  const char* rightHost;
  
}player_t;
