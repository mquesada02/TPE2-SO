GLOBAL prepare_process

section .text


%macro pushState 0
	push qword 0x0  ;rax
	push qword 0x0  ;rbx
	push qword 0x0  ;rcx
	push qword 0x0  ;rdx
	push rdi        ;rbp
	push rdx  		;rdi
	push rcx  		;rsi
	push qword 0x0  ;r8
	push qword 0x0  ;r9
	push qword 0x0  ;r10
	push qword 0x0  ;r11
	push qword 0x0  ;r12
	push qword 0x0  ;r13
	push qword 0x0  ;r14
	push qword 0x0  ;r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro
    
prepare_process:
	
	push rbp
	mov rbp, rsp

    mov rsp, rdi ;recibe la direccion de la memoria
	
    push dword 0x0      ;SS
    push rdi            ;RSP como primer parámetro el inicio del stack
    push qword 0x202    ;RFLAGS
    push dword 0x8      ;CS
    push rsi            ;RIP como segundo paramentro el puntero al proceso que se quiere correr
    pushState

    mov rax, rsp

	mov rsp, rbp
	pop rbp

    ret
