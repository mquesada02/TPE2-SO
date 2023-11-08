GLOBAL sem_wait_asm

EXTERN runningPID
EXTERN blockProcess

section .text

sem_mutex:
    ;en rdi esta la direccion del mutex del semaforo
    ;el runningPID es el proceso que quiere entrar al semaforo
    ;si ya esta accediendo ese mismo semaforo otro proceso, se bloquea
    mov rax, 1
    xchg rax, [rdi] 
    cmp rax, 0
    je .end
    mov rdi, runningPID
    blockProcess
.end:
    ret
