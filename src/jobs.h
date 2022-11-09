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

void bg_command_finished(int sig);
void jobs(void);
size_t compute_needed_size(char ***seq);
void copy_seq(char **cmd, char ***seq);
#endif // JOBS_H
