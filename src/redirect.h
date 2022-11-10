#ifndef REDIRECT_H
#define REDIRECT_H 
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Create a son that reads a file and send it to the stdin of the father processus */
void read_file(char *file);

/* Create a son that open that reads from a pipe and writes the output in a file */
void write_file(char *file);

#endif // REDIRECT_H
