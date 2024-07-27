
enum _expr_rule {
  R_NB = 'N', R_BOOL = 'B', R_ID = 'I',
  R_ASSIGN = 'E',
  R_ADDI = '+', R_SOUS = '-', R_MULT = '*', R_DIVI = '/', R_MOINS = 'M',
  R_EQUALS = '=', R_INF_EQ = 'W', R_INF = '<',
  R_AND = '&', R_NOT = '!'
};

enum _comm_rule {
  R_EXPR = 'E', R_IMPORT = 'I', R_IF = 'C', R_DO = 'D', R_BR = '{'
};

/* unary-and-binary tree structure */
struct _expr_tree {
  char rule;                    /* "name" of the rule/operation operation */
  double number;                   /* int  for value */
  unsigned int boolean;
  char *identifiant;
  unsigned int size;
  struct _expr_tree* left;           /* NULL if unary node or leaf*/
  struct _expr_tree* right;          /* used for unary node but NULL if leaf */
};

typedef struct _expr_tree* AST_expr;

struct _command_tree {
  char rule;                    /* "name" of the rule/operation operation */
  AST_expr expr1;     	        /* used for command with at least one sub-expression */
  struct _command_tree *comm1;
  struct _command_tree *comm2;
  unsigned int size;
  struct _command_list* prog;
};

typedef struct _command_tree* AST_comm;

/* return the size (in number of lines) of an expression */
unsigned int size_expr(const char rule, const AST_expr left, const AST_expr right);
unsigned int size_comm(const char rule, const AST_expr expression, const AST_comm comm1, const AST_comm comm2, struct _command_list* prog);

/* create an AST from a root value and two AST sons */
AST_expr new_binary_expr(char rule, AST_expr left, AST_expr right);

/* create an AST from a root value and one AST son */
AST_expr new_unary_expr(char rule, AST_expr son);

/* create an AST leaf from a value */
AST_expr new_leaf_expr(const char rule, const double number, const unsigned int boolean, const char *identifiant);

/* create an AST leaf from a value */
AST_comm new_command(const char rule, AST_expr expression, AST_comm comm1, AST_comm comm2, struct _command_list* prog);

/* delete an AST */
void free_expr(AST_expr t);
void free_comm(AST_comm t);

/* print an AST*/
void print_expr(AST_expr t);
void print_comm(AST_comm t);

void code_comm(AST_comm t);
void code_expr(AST_expr t);
void import_code(char* name);
int size_import(char* name);
void semantic_analysis(AST_expr t);