struct Node{ 
  int val;
  struct Node* next;
};

typedef struct Node Node_t;

struct potato{
  int hops;
  int ID;
  struct potato* ID_head;
};
  
typedef struct potato potato_t;
