

struct potato{
  // track the current hops number in the game
  int hops;
  // is a ID queue to trace the ID sending the potato
  int ID[1024];
};
  
typedef struct potato potato_t;
