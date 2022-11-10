#ifndef JOBS_H
#define JOBS_H
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "jobslinkedlist.h"

/* Delete the background command process once finished */
//void bg_command_finished(int sig);
void bg_command_finished(int signo, siginfo_t *, void *) ;

/* The jobs command: it prints pid and associated text of the background commands that are currently running */
void jobs(void);

/* Count the total size of the given sequence */
size_t compute_needed_size(char ***seq);

/* Build the command text associated to the given sequence */
void copy_seq(char **cmd, char ***seq);
#endif // JOBS_H
