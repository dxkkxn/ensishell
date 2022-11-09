#ifndef REDIRECT_H
#define REDIRECT_H 
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Set the file descriptor of the given file as standard input to be read */
void read_file(char *file);

/* Set the file descriptor of the given file as standard output to write inside */
void write_file(char *file);
#endif // REDIRECT_H
