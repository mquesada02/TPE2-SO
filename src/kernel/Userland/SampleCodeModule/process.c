#include <syscalls.h>

void startProcess(int priority, void (* process), char argc, char* argv[]) {
    syscall_startProcess(priority, process, argc, argv);
}
