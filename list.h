struct _command_node {
  struct _command_node *next;
  AST_comm comm;
};

typedef struct _command_node* ND_comm;

struct _command_list {
	ND_comm first;
	ND_comm last;
};

typedef struct _command_list* LST_comm;

LST_comm new_queue();
void free_queue(LST_comm list);

void enqueue(LST_comm list, AST_comm comm);
AST_comm dequeue(LST_comm list);

void print_queue(LST_comm list);

int size_queue(LST_comm list);
void code_queue(LST_comm prog);