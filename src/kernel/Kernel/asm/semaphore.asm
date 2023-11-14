GLOBAL sem_lock_wait
GLOBAL sem_lock_post

EXTERN getRunningPID
EXTERN blockSemProcess
EXTERN unblockSemProcess

section .text
sem_lock_wait:
    ;en rdi esta la direccion del lock del semaforo
    ;en rsi esta el semaforo
    ;el runningPID es el proceso que quiere entrar al semaforo
    ;si ya esta accediendo ese mismo semaforo otro proceso, se bloquea
    mov r10, rdi ;direccion del lock
    mov r11, rsi ;semaforo
    mov qword rbx, 0
    xchg bl, [rdi] 
    cmp bl, 1
    je end
    mov rdi, rsi ;semaforo
    mov rsi, 1 ;lo que esta esperando es el lock, no el semaforo en si -> waiting_locked = 1
    call blockSemProcess
    mov rdi, r10
    mov rsi, r11
    jmp sem_lock_wait
end:
    ret


sem_lock_post:
    ;en rdi esta el lock
    mov byte [rdi], 1
    mov rdi, rsi ;el semaforo
    mov rsi, 1 ; waiting_locked = 1 -> que se desbloquee a los que estan esperando el lock y no el semaforo en si
    call unblockSemProcess

    
