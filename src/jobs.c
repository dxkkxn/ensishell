#include "jobs.h"

extern node_t *head;
void bg_command_finished(int signo, siginfo_t *x, void *y) {
  errno = 0;
  pid_t exited_process;
  while ((exited_process = waitpid(-1, NULL, WNOHANG)) > 0) {
    printf("bg cmd pid: %d\n", exited_process);
    node_t *exited_node;
    if ((exited_node = in(exited_process, head))) {
      printf("[%d] %d done     %s\nensishell>", exited_node->num,
             exited_process, exited_node->val.cmd);
      fflush(stdout);
      delete (&head, exited_node);
    }
  }
}

void jobs(void) {
  node_t *curr = head;
  while (curr != NULL) {
    printf("[%d] %d running     %s\n", curr->num, curr->val.pid, curr->val.cmd);
    curr = curr->next;
  }
}

size_t compute_needed_size(char ***seq) {
  int i = 0;
  size_t res = 0;
  int j = 0;
  while (seq[i] != NULL) {
    j = 0;
    while (seq[i][j] != NULL) {
      res += strlen(seq[i][j]) + 1; // + 1 to add a space between words
      j++;
    }
    res += 2; // pipe symbol and a space
    i++;
  }
  return res - 3; // remove last pipe symbol and 2 spaces
}

void copy_seq(char **cmd, char ***seq) {
  size_t size = compute_needed_size(seq);
  *cmd = malloc(sizeof(char) * size + 1); // + 1 '\0' of last char
  int k = 0;
  int i = 0; // cmd index
  int j;
  while (seq[k] != NULL) {
    j = 0; // cmd2 index
    while (seq[k][j] != NULL) {
      strcpy(*cmd + i, seq[k][j]);
      i += strlen(seq[k][j]);
      (*cmd)[i++] = ' ';
      j++;
    }
    if (seq[k + 1] != NULL) {
      (*cmd)[i++] = '|';
      (*cmd)[i++] = ' ';
    }
    k++;
  }
  (*cmd)[size] = '\0';
}
