#ifndef JOBSLINKEDLIST_H_
#define JOBSLINKEDLIST_H_
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

struct bg_cmd {
  pid_t pid;
  char *cmd;
};

typedef struct node_t {
  struct bg_cmd val;
  int num;
  struct node_t* next;
} node_t;

/*  */
void push(node_t** head, struct bg_cmd cmd);

/*  */
int length(node_t* head);

/*  */
void delete(node_t** head, node_t * node);

/*  */
node_t* in(pid_t pid, node_t * head);








#endif // JOBSLINKEDLIST_H_
