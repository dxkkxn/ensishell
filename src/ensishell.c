/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "readcmd.h"
#include "redirect.h"
#include "jobs.h"
#include "variante.h"

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

node_t *head = NULL;
/*
 *  execute all commands passed as parameter
 *
 */


void execute_command(char **command) {
  // for now execute just the first
  if (strcmp(command[0], "jobs") == 0) {
    jobs();
    exit(EXIT_SUCCESS);
  } else {
    execvp(command[0], command);
    perror("execvp failed :");
    exit(EXIT_FAILURE);
  }
}

int length_seq(char ***seq) {
  int i = 0;
  while (seq[i] != NULL)
    i++;
  return i;
}
void close_unused_pipes(int **pipes, int n) {
  for (int j = 0; j < n; j++) {
    close(pipes[j][0]);
    close(pipes[j][1]);
  }
}
int **alloc_and_init_pipes(unsigned int n) {
  if (n == 0)
    return NULL;
  int **pipes = malloc(sizeof(int) * (n));
  for (int i = 0; i < n; i++) {
    pipes[i] = malloc(sizeof(int) * 2);
    pipe(pipes[i]);
  }
  return pipes;
}
void free_pipes(int **pipes, int n) {
  for (int i = 0; i < n; i++) {
    free(pipes[i]);
  }
  free(pipes);
}
void execute_sequence(struct cmdline *commands) {
  if (commands->err != NULL) {
    fprintf(stderr, "ERROR %s\n", commands->err);
    return;
  }
  int n = length_seq(commands->seq);
  int **pipes = alloc_and_init_pipes(n - 1); // we need n-1 pipes
  pid_t pid_son;
  pid_t *all_sons = malloc(sizeof(pid_t) * n);
  for (int i = 0; i < n; i++) {
    if ((pid_son = xfork()) == 0) {
      if (i > 0)
        dup2(pipes[i - 1][0], STDIN_FILENO);
      if (i < n - 1)
        dup2(pipes[i][1], STDOUT_FILENO);
      if (i == 0 && commands->in != NULL) {
        close_unused_pipes(pipes, n - 1);
        read_file(commands->in);
      }
      if (i == n - 1 && commands->out != NULL) {
        close_unused_pipes(pipes, n - 1);
        write_file(commands->out);
      }
      // son closes all open (unused) pipes
      close_unused_pipes(pipes, n - 1);
      execute_command(commands->seq[i]);
      assert(1 == 0); // line never executed;
    }
    all_sons[i] = pid_son;
  }
  // father closses all open unused pipes
  close_unused_pipes(pipes, n - 1);
  if (commands->bg) {
    struct bg_cmd new_cmd;
    new_cmd.pid = pid_son;
    new_cmd.cmd = NULL;
    copy_seq(&new_cmd.cmd, commands->seq);
    assert(new_cmd.cmd != NULL);
    push(&head, new_cmd);
  } else {
    for (int i = 0; i < n; i++)
      waitpid(all_sons[i], NULL, 0);
  }
  free_pipes(pipes, n - 1);
  free(all_sons);
}

int question6_executer(char *line) {
  /* Question 6: Insert your code to execute the command line
   * identically to the standard execution scheme:
   * parsecmd, then fork+execvp, for a single command.
   * pipe and i/o redirection are not required.
   */
  struct cmdline *cmd = parsecmd(&line);
  execute_sequence(cmd);

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

  struct sigaction sa;
  sa.sa_handler = &bg_command_finished;
  sigaction(SIGCHLD, &sa, NULL);
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

    execute_sequence(l);

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
