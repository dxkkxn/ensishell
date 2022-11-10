#ifndef JOBSLINKEDLIST_H_
#define JOBSLINKEDLIST_H_
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

/* Structure that represents a background command */
struct bg_cmd {
  pid_t pid;
  char *cmd;
};

/* Define the type node as an element of the background linked list  */
typedef struct node_t {
  struct bg_cmd val;
  int num;
  struct node_t* next;
} node_t;

/* Add a new node to the beginning of the linked list */
void push(node_t** head, struct bg_cmd cmd);

/* Count the nodes inside the list */
int length(node_t* head);

/* Delete the given node from the list */
void delete(node_t** head, node_t * node);

/* Return the node of pid if pid in the linked list null if not */
node_t* in(pid_t pid, node_t * head);








#endif // JOBSLINKEDLIST_H_
