GLOBAL sem_lock_wait
GLOBAL sem_lock_post

EXTERN getRunningPID
EXTERN blockSemProcess
EXTERN unblockSemProcess

section .text
sem_lock_wait:
    ;en rdi esta la direccion del lock del semaforo
    ;el runningPID es el proceso que quiere entrar al semaforo
    ;si ya esta accediendo ese mismo semaforo otro proceso, se bloquea
    mov r10, rdi
    mov rax, 0
    xchg rax, [rdi] 
    cmp rax, 1
    je end
    mov rsi, rdi
    call getRunningPID
    mov rdi, rax
    call blockSemProcess
    mov rdi, r10
    jmp sem_lock_wait
end:
    ret


sem_lock_post:
    ;en rdi esta el lock
    mov dword [rdi], 0
    call getRunningPID
    mov rdi, rax
    call unblockSemProcess

    
