/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#include "readcmd.h"
#include "variante.h"
#include "jobslinkedlist.h"


#ifndef VARIANTE
#error "Variante non défini !!"
#endif

/* Guile (1.8 and 2.0) is auto-detected by cmake */
/* To disable Scheme interpreter (Guile support), comment the
 * following lines.  You may also have to comment related pkg-config
 * lines in CMakeLists.txt.
 */

#if USE_GUILE == 1
#include <libguile.h>

pid_t xfork() {
  pid_t res;
  if ((res = fork()) == -1) {
    perror("fork error :");
    exit(1);
  }
  return res;
}


node_t * head = NULL;
/*
 *  execute all commands passed as parameter
 *
*/

void jobs(void) {
  node_t * curr = head;
  int n = length(head);
  int i = 0;
  while (curr != NULL) {
    int wstatus;
    waitpid(curr->val.pid, &wstatus, WNOHANG);
    if (WIFEXITED(wstatus)) {
      printf("[%d] Done pid : %d command : %s\n", n-i, curr->val.pid, curr->val.cmd);
      delete(&head, curr);
    } else {
      printf("[%d] Running  pid : %d command : %s\n", n-i, curr->val.pid, curr->val.cmd);
    }
    curr = curr->next;
    i++;
  }
}

size_t compute_needed_size(char **cmd) {
  int i = 0;
  size_t res = 0;
  while (cmd[i] != NULL) {
    res += strlen(cmd[i]) + 1; // + 1 to add a space between words
    i++;
  }
  return res - 1; // -1 to delete the useless last space
}

void copy_cmd(char ** cmd, char **cmd2) {
  size_t size = compute_needed_size(cmd2);
  *cmd = malloc(sizeof(char) * size + 1); // + 1 '\0' of last char
  int i = 0; // cmd index
  int j = 0; //cmd2 index
  while (cmd2[j] != NULL) {
    strcpy(*cmd + i, cmd2[j]);
    i += strlen(cmd2[j]);
    (*cmd)[i++] = ' ';
    j++;
  }
  (*cmd)[size] = '\0';
}
void execute_command(struct cmdline * commands) {
  // for now execute just the first
  char** curr = commands->seq[0];
  if (strcmp(curr[0], "jobs") == 0) {
    return jobs();
  }
  pid_t pid_son;
  if ((pid_son = xfork()) == 0) {
    //son
    execvp(curr[0], curr);
    assert(1==0); // line never executed
  }
  if (!commands->bg) {
    waitpid(pid_son, NULL, 0);
  } else {
    struct bg_cmd new_cmd;
    new_cmd.pid = pid_son;
    new_cmd.cmd = NULL;
    copy_cmd(&new_cmd.cmd, curr);
    assert(new_cmd.cmd != NULL);
    push(&head, new_cmd);
  }

}
int question6_executer(char *line) {
  /* Question 6: Insert your code to execute the command line
   * identically to the standard execution scheme:
   * parsecmd, then fork+execvp, for a single command.
   * pipe and i/o redirection are not required.
   */
  printf("Not implemented yet: can not execute %s\n", line);

  /* Remove this line when using parsecmd as it will free it */
  free(line);

  return 0;
}

SCM executer_wrapper(SCM x) {
  return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}
#endif

void terminate(char *line) {
#if USE_GNU_READLINE == 1
  /* rl_clear_history() does not exist yet in centOS 6 */
  clear_history();
#endif
  if (line)
	free(line);
  printf("exit\n");
  exit(0);
}

int main() {
  printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
  scm_init_guile();
  /* register "executer" function in scheme */
  scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif

  while (1) {
	struct cmdline *l;
	char *line = 0;
	int i, j;
	char *prompt = "ensishell>";

	/* Readline use some internal memory structure that
	   can not be cleaned at the end of the program. Thus
	   one memory leak per command seems unavoidable yet */
	line = readline(prompt);
	if (line == 0 || !strncmp(line, "exit", 4)) {
	  terminate(line);
	}

#if USE_GNU_READLINE == 1
	add_history(line);
#endif

#if USE_GUILE == 1
	/* The line is a scheme command */
	if (line[0] == '(') {
	  char catchligne[strlen(line) + 256];
	  sprintf(catchligne,
			  "(catch #t (lambda () %s) (lambda (key . parameters) (display "
			  "\"mauvaise expression/bug en scheme\n\")))",
			  line);
	  scm_eval_string(scm_from_locale_string(catchligne));
	  free(line);
	  continue;
	}
#endif

	/* parsecmd free line and set it up to 0 */
	l = parsecmd(&line);



	/* If input stream closed, normal termination */
	if (!l) {

	  terminate(0);
	}

	execute_command(l);

	if (l->err) {
	  /* Syntax error, read another command */
	  printf("error: %s\n", l->err);
	  continue;
	}

	if (l->in)
	  printf("in: %s\n", l->in);
	if (l->out)
	  printf("out: %s\n", l->out);
	if (l->bg)
	  printf("background (&)\n");

	/* Display each command of the pipe */
	for (i = 0; l->seq[i] != 0; i++) {
	  char **cmd = l->seq[i];
	  printf("seq[%d]: ", i);
	  for (j = 0; cmd[j] != 0; j++) {
		printf("'%s' ", cmd[j]);
	  }
	  printf("\n");
	}
  }
}
