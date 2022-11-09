#include "redirect.h"

extern pid_t xfork();

void read_file(char *file) {
  int rdin_pipe[2];
  pipe(rdin_pipe);
  if (xfork() == 0) {
    close(rdin_pipe[0]);
    // open and read
    int fd;
    char buf[1024];
    fd = open(file, O_RDONLY);
    ssize_t size;
    while ((size = read(fd, buf, 1024)) > 0) {
      write(rdin_pipe[1], buf, size);
    }
    close(rdin_pipe[1]);
    exit(EXIT_SUCCESS);
  }
  dup2(rdin_pipe[0], STDIN_FILENO);
  close(rdin_pipe[0]);
  close(rdin_pipe[1]);
}

void write_file(char *file) {
  int rdout_pipe[2];
  pipe(rdout_pipe);
  if (xfork() == 0) {
    close(rdout_pipe[1]);
    // open and read
    int fd;
    char buf[1024];
    fd = open(file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(fd, 0);
    ssize_t size;
    while ((size = read(rdout_pipe[0], buf, 1024)) > 0) {
      write(fd, buf, size);
    }
    close(rdout_pipe[0]);
    exit(EXIT_SUCCESS);
  }
  dup2(rdout_pipe[1], STDOUT_FILENO);
  close(rdout_pipe[0]);
  close(rdout_pipe[1]);
}
