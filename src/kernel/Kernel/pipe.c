#include <pipe.h>
#include <lib.h>

int current_pipe = 2;

int pipe(int pipefd[2]) {
    if (current_pipe >= MAX_FDS) {
        return -1;
    }
    pipefd[0] = current_pipe;
    pipefd[1] = current_pipe + 1;
    current_pipe += 2;
    return 0;
}

