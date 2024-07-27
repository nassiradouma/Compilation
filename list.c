#include<stdlib.h>
#include "AST.h"
#include "list.h"

LST_comm new_queue() {
	LST_comm result = malloc(sizeof(struct _command_list));
	if(result != NULL)
		result->first = result->last = NULL;
	return result;
}

void enqueue(LST_comm list, AST_comm comm) {
	if(list != NULL && comm != NULL) {
		ND_comm node = malloc(sizeof(struct _command_node));
			if(node != NULL) {
				node->comm = comm;
				node->next = NULL;
				if(list->first == NULL)
					list->first = list->last = node;
				else {
					list->last->next = node;
					list->last = node;
			}
		}
	}
}

AST_comm dequeue(LST_comm list) {
	AST_comm result = NULL;
	if(list != NULL) {
		ND_comm temp = list->first;
		if(list->last == list->first)
			list->last = NULL;
		if(list->first != NULL) {
			result = list->first->comm;
			list->first = list->first->next;
		} free(temp);
	} return result;
}

void print_queue(LST_comm list) {
	if(list != NULL)
		for(ND_comm pos = list->first; pos != NULL; pos = pos->next)
			print_comm(pos->comm);
}

void free_queue(LST_comm list) {
	if(list != NULL) {
		ND_comm pos = list->first, temp;
		while(pos != NULL) {
			free_comm(pos->comm);
			temp = pos;
			pos = pos->next;
			free(temp);
		}
	} free(list);
}

void code_queue(LST_comm prog) {
	if(prog != NULL) {
		AST_comm comm;
		while((comm = dequeue(prog)) != NULL)
			code_comm(comm);
	}
}

int size_queue(LST_comm list) {
	int result = 0;
	if(list != NULL)
		for(ND_comm pos = list->first; pos != NULL; pos = pos->next)
			if(pos->comm != NULL)
				result += pos->comm->size;
	return result;
}