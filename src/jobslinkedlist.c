#include "jobslinkedlist.h"

int length(node_t* head) {
    int len = 0;
    node_t * curr = head;
    while(curr != NULL) {
        len++;
        curr = curr->next;
    }
    return len;

}
void push(node_t** head, struct bg_cmd cmd){
    node_t* new_head = malloc(sizeof(node_t)) ;
    int num = length(*head);
    if (new_head == NULL) {
        perror("malloc push :");
        exit(1);
    }
    new_head->next = *head;
    new_head->val = cmd;
    new_head->num = num+1;
    *head = new_head;
    return;
}
void delete(node_t** head, node_t * node) {
    node_t * curr = *head;
    node_t * prev = NULL;
    while (curr != node) {
        prev = curr;
        curr = curr->next;
        assert(curr != NULL); // If curr is NULL node is not in the list;
    }
    if (prev == NULL) {
        // the node is the head;
        *head = (*head)->next;
    } else {
        prev->next = curr->next;
    }
    free(curr->val.cmd);
    free(curr);
    return;
}

node_t* in(pid_t pid, node_t * head) {
    node_t * curr = head;
    while (curr != NULL && curr->val.pid != pid) {
        curr = curr->next;
    }
    return curr;
}
