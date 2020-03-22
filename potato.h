struct Node{ 
  int val;
  struct Node* next;
};

typedef struct Node Node_t;

struct potato{
  int hops;
  Node_t* ID_head;
};
  
typedef struct potato potato_t;
