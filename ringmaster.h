#include "potato.h"

typedef struct ringmaster{
  potato_t Potato;
  int portNum;
  int numPlayers;
  int numHops;
}ringmaster_t;
