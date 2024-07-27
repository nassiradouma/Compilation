#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "AST.h"
#include "list.h"

/* return the size (in number of lines) of an expression */
unsigned int size_expr(const char rule, const AST_expr left, const AST_expr right) {
  unsigned int result = 0;
  if(left != NULL)
    result += left->size;
  if(right != NULL)
    result += right->size;

  switch(rule) {
    case R_AND: return result + 3;
    default: return result + 1;
  }
}

unsigned int size_comm(const char rule, const AST_expr expression, const AST_comm comm1, const AST_comm comm2, struct _command_list* prog) {
  unsigned int result = 0;
  if(expression != NULL)
    result += expression->size;
  if(comm1 != NULL)
    result += comm1->size;
  if(comm2 != NULL)
    result += comm2->size;
  if(prog != NULL)
    result += size_queue(prog);

  switch(rule) {
    case R_IMPORT: return size_import(expression->identifiant);
    case R_DO:
    case R_IF: return result + 2;
    case R_EXPR: return result + 1;
    default: return result;
  }
}

int size_import(char* name) {
  int result = 0;
  FILE* temp = fopen(name,"r");
  char buffer;
  while(fread(&buffer,1,1,temp)==1)
    if(buffer == '\n') result++;
  if(buffer != '\n')
    result++;
  return result;
}

/* create an AST from a root value and two AST sons */
AST_expr new_binary_expr(char rule, AST_expr left, AST_expr right) {
  AST_expr t=(struct _expr_tree*) malloc(sizeof(struct _expr_tree));
  if (t!=NULL){	/* malloc ok */
    t->rule=rule;
    t->left=left;
    t->right=right;
    t->size=size_expr(rule,left,right);
  } else printf("ERR : MALLOC ");
  return t;
}

/* create an AST from a root value and one AST son */
AST_expr new_unary_expr(char rule, AST_expr son)
{
  return new_binary_expr(rule, NULL,son);
}

/* create an AST leaf from a value */
AST_expr new_leaf_expr(const char rule, const double number, const unsigned int boolean, const char *identifiant) {
  AST_expr t=(struct _expr_tree*) malloc(sizeof(struct _expr_tree));
  if(t!=NULL) {
    t->rule = rule;
    t->size = 1;
    t->left = t->right = NULL;
    t->identifiant = NULL;

    switch(rule) {
      case R_NB: t->number = number; return t;
      case R_BOOL: t->boolean = boolean; return t;
      case R_ID: t->identifiant = malloc(256);
        if(t->identifiant == NULL) {
          printf("ERR : MALLOC ");
          return NULL;
        } strncpy(t->identifiant,identifiant,256);
        return t;
    }

  } else printf("ERR : MALLOC\n");
  return t;
}

/* create an AST leaf from a value */
AST_comm new_command(const char rule, AST_expr expression, AST_comm comm1, AST_comm comm2, struct _command_list* prog){
  AST_comm t =  malloc(sizeof(struct _command_tree));
  if (t!=NULL){	/* malloc ok */
    t->rule = rule;
    t->expr1 = expression;
    semantic_analysis(t->expr1);
    t->comm1 = comm1;
    t->comm2 = comm2;
    t->prog = prog;
    t->size=size_comm(rule,expression,comm1,comm2,prog);
  } else printf("ERR : MALLOC ");
  return t;
}

/* delete an AST */
void free_expr(AST_expr t)
{
  if (t!=NULL) {
    free_expr(t->left);
    free_expr(t->right);
    free(t->identifiant);
    free(t);
  }
}
void free_comm(AST_comm t)
{
  if (t!=NULL) {
    free_expr(t->expr1);
    free_queue(t->prog);
    free(t);
  }
}

/* infix print an AST*/
void print_expr(AST_expr t){
  if (t!=NULL) {
    printf("[ ");
    print_expr(t->left);
    if (t->left==NULL) printf(":%f: ",t->number); else printf(":%c: ",t->rule);
    print_expr(t->right);
    printf("] ");
  }
}

void print_comm(AST_comm t){
  if (t!=NULL) {
    printf("[ ");
    printf(":%c: ",t->rule);
    print_expr(t->expr1);
    if(t->rule == R_IF) {
      printf("THEN ");
      print_comm(t->comm1);
      printf("ELSE ");
      print_comm(t->comm2);
    }
    if(t->rule == R_DO) {
      print_comm(t->comm1);
    }
    if(t->rule == R_BR) {
      print_queue(t->prog);
    }
    printf("] ");
  }
}

void code_comm(AST_comm t) {
  if(t!=NULL) {
    switch(t->rule) {
      case R_EXPR: code_expr(t->expr1); printf("Drop\n"); break;
      case R_IMPORT: if(t->expr1 != NULL) import_code(t->expr1->identifiant); break;
      case R_IF:
        if(t->expr1 != NULL && t->comm1 != NULL && t->comm2 != NULL) {
          code_expr(t->expr1);
          printf("CondJmp %d\n",t->comm1->size+1);
          code_comm(t->comm1);
          printf("Jump %d\n",t->comm2->size);
          code_comm(t->comm2);
        } break;
      case R_DO:
        if(t->expr1 != NULL && t->comm1) {
          code_comm(t->comm1);
          code_expr(t->expr1);
          printf("Not\n");
          printf("CondJmp %d\n", -(t->comm1->size + t->expr1->size + 2));
        } break;
      case R_BR:
        if(t->prog != NULL)
          code_queue(t->prog);
    }
  }
}

/* suffix print an AST*/
void code_expr(AST_expr t) {
  if(t!=NULL) {
    if(t->rule != R_AND && t->rule != R_ASSIGN) {
      code_expr(t->left);
      code_expr(t->right);      
    }
    switch(t->rule) {
      case R_NB: 
        if(isnan(t->number))  printf("CsteNb NaN\n");
        else                  printf("CsteNb %f\n",t->number);
        break;
      case R_BOOL:
        if(t->boolean) printf("CsteBo True\n");
        else printf("CsteBo False\n");
        break;
      case R_INF_EQ: printf("LoEqNb\n"); break;
      case R_EQUALS: printf("Equals\n"); break;
      case R_INF: printf("LoStNb\n"); break;
      case R_NOT: printf("Not\n"); break;
      case R_ADDI: printf("AddiNb\n"); break;
      case R_SOUS: printf("SubiNb\n"); break;
      case R_MULT: printf("MultNb\n"); break;
      case R_DIVI: printf("DiviNb\n"); break;
      case R_MOINS: printf("NegaNb\n"); break;
      case R_ASSIGN:
        code_expr(t->right);
        if(t->left != NULL) {
          printf("SetVar %s\n",t->left->identifiant);
          printf("GetVar %s\n",t->left->identifiant);
        } break;
      case R_ID: printf("GetVar %s\n",t->identifiant); break;
      case R_AND:
        code_expr(t->left); printf("ConJmp %d\n", t->right->size +1);
        code_expr(t->right); printf("Jump 1\n"); printf("CsteBo False\n");
        break;
    }
  }
}

void import_code(char* name) {
  FILE* temp = fopen(name,"r");
  char buffer;
  while(fread(&buffer,1,1,temp)==1)
    printf("%c",buffer);
  if(buffer != '\n')
    printf("\n");
}

void semantic_analysis(AST_expr t) {
  if(t!=NULL) {
    /* resursive analysis*/
    semantic_analysis(t->left);
    semantic_analysis(t->right);
    t->size = size_expr(t->rule,t->left,t->right);
    /* not a leaf */
    if(t->right != NULL) {
      
      /* binary expression */
      if(t->left != NULL) {

        /* Nb OP Nb */
        if(t->left->rule == R_NB && t->right->rule == R_NB)
          switch(t->rule) {

            /* arithmetic */
            case R_ADDI:
              t->rule = R_NB; t->number = t->left->number + t->right->number; t->size = 1;
              free_expr(t->left); free_expr(t->right); t->left = t->right = NULL; return;
            case R_SOUS:
              t->rule = R_NB; t->number = t->left->number - t->right->number; t->size = 1;
              free_expr(t->left); free_expr(t->right); t->left = t->right = NULL; return;
            case R_MULT:
              t->rule = R_NB; t->number = t->left->number * t->right->number; t->size = 1;
              free_expr(t->left); free_expr(t->right); t->left = t->right = NULL; return;
            case R_DIVI:
              t->rule = R_NB; t->number = t->left->number / t->right->number; t->size = 1;
              free_expr(t->left); free_expr(t->right); t->left = t->right = NULL; return;

            /* Comparison */
            case R_INF:
              t->rule = R_BOOL; t->number = t->left->number < t->right->number; t->size = 1;
              free_expr(t->left); free_expr(t->right); t->left = t->right = NULL; return;
            case R_INF_EQ:
              t->rule = R_BOOL; t->number = t->left->number <= t->right->number; t->size = 1;
              free_expr(t->left); free_expr(t->right); t->left = t->right = NULL; return;           
            case R_EQUALS:
              t->rule = R_BOOL; t->number = t->left->number == t->right->number; t->size = 1;
              free_expr(t->left); free_expr(t->right); t->left = t->right = NULL; return;
            default: return;
          }
        /* Bool OP Bool*/
        if(t->left->rule == R_BOOL && t->right->rule == R_BOOL && t->rule == R_EQUALS) {
          t->rule = R_BOOL; t->boolean = t->left->boolean == t->right->boolean; t->size = 1;
          t->left = t->right = NULL; free_expr(t->left); free_expr(t->right);
        } return;
      }

      /* unary expression */
      else {
        /* replace number by -number */
        if(t->right->rule == R_NB) {
          if(t->rule == R_MOINS) {
            t->rule = R_NB; t->number = -(t->right->number);
            free_expr(t->right); t->right = NULL; t->size = 1;
          } return;
        }
        /* replace true by false or false by true */
        if(t->right->rule == R_BOOL)
          if(t->rule == R_NOT) {
            t->rule = R_BOOL; t->boolean = !(t->right->boolean);
            free_expr(t->right); t->right = NULL; t->size = 1;
          }
      }
      /* end of unary expression */
    }
  }
}