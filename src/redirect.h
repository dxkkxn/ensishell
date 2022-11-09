#ifndef REDIRECT_H
#define REDIRECT_H 
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void read_file(char *file);
void write_file(char *file);
#endif // REDIRECT_H
