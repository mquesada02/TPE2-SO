#include <stdint.h>
#include <processes.h>
#include <interrupts.h>
#include <videodriver.h>
#include <lib.h>
#include <scheduler.h>


typedef struct file_descriptor {
    char fdbuff[FD_SIZE];
    int pos;
} fd_t;

fd_t fd_table[MAX_FDS];


extern char buffer;


void initFDs() {
    for (int i=0;i<MAX_FDS;i++) {
        fd_table[i].pos = 0;
        for (int j=0;j<FD_SIZE;j++) {
            fd_table[i].fdbuff[j] = 0;
        }
    }
}

/**
 * @brief Copia un caracter las veces deseadas sobre un puntero dado
 * 
 * @param destination Puntero donde se desea copiar el caracter
 * @param c Caracter que se desea copiar sobre el puntero
 * @param length Veces que se desea copiar el caracter sobre el puntero
 * @return El puntero reemplazado
 */
void * memset(void * destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

/**
 * @brief Escribe sobre la pantalla un caracter con su color deseado.
 * 
 * @param c Caracter al cual se quiere escribir sobre la pantalla.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @param BGColor Color de fondo del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 */
void write(int fd, unsigned char c, int FGColor, int BGColor) {
    if (!isOpenedFD(getRunningPID(),fd)) return;
    if (fd == 1 && c != 255 /*-1*/) {
        drawChar(c, FGColor, BGColor);
    } else {
        /* write in fd_table */
        writeByteFD(fd, c);
        /* unblock process waiting for that fd */
        for(int i=1; i<MAX_PROCESSES; i++) {
            unblockFD(i, fd);
        }
    }
        
}

/**
 * @brief Copia la cantidad de caracteres deseados desde un puntero fuente sobre un puntero destino.
 * 
 * @param destination 
 * @param source 
 * @param length 
 * @return void* 
 */
void * memcpy(void * destination, const void * source, uint64_t length) {
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

/**
 * @brief Convierte un número de hasta 8 bytes en su representación ASCII en la base deseada
 * sobre una cadena de caracteres proporcionado. Acepta valores negativos.
 * 
 * @param num Número que se desea representar. Bases soportadas: decimal y hexadecimal.
 * @param base Base a convertir el número proporcionado
 * @param buffer Cadena de caracteres ya realocada para sobrescribir sobre ella la representación del número
 * en caracteres ASCII.
 */
void numToStr(int64_t num, int base, char * buffer) {
    /* calculo cuantos dígitos tiene */
    /* BASE != 0 (hay que verificar?)*/
    int i=0;
    int isPositive = num >= 0;
    if (!isPositive) {
        buffer++; /* reservo un lugar */
        num*=-1;
    } else if (num==0){ /* caso especial de si el número es 0*/
        buffer[0]='0';
        buffer[1]='\0';
        return;
    }
        
    uint64_t aux = num;
    while(aux!=0) {
        i++;
        aux/=base;
    }
    
    buffer[i--] = '\0';

    int mod=0;
    while(i>=0){
        mod = num%base;
        if (mod<10) {
            buffer[i] = mod + '0';
        } else {
            /* hex */
            buffer[i] = mod - 10 + 'A';
        }
        num/=base;
        i--;
    }
    if (!isPositive) {
        buffer--;
        buffer[0]='-';
    }
        
}

void moveFDCursor(int fd) {
    for(int i=0;i<fd_table[fd].pos;i++) {
        fd_table[fd].fdbuff[i] = fd_table[fd].fdbuff[i+1];
    }
    fd_table[fd].pos--;
    return;
}

void sendEOF(int fd) {
    fd_table[fd].fdbuff[fd_table[fd].pos] = -1;
    fd_table[fd].pos++;
    return;
}


/**
 * @brief Retorna el valor leído de la entrada de teclado.
 * 
 * @return Valor leído de la entrada de teclado.
 */
/**
 * @brief Retorna el valor ASCII guardado en la variable buffer que se modifica con la interrupción 21h.
 * 
 * @return Valor ASCII guardado en la variable buffer.
 */
unsigned char read(int fd) {
    if (!isOpenedFD(getRunningPID(), fd) || fd_table[fd].pos == 0) {
        /* block process until new arrival */
        blockFD(getRunningPID(), fd);
        _stint20();
    } else {
        char toReturn = fd_table[fd].fdbuff[0]; 
        moveFDCursor(fd);
        return toReturn;
    }
    return read(fd); // cuando se desbloquea un proceso, sigue estando en 0 la posición, por lo tanto, se necesita llamar devuelta
    
    // if (fd == 0) {
    //     buffer = 0;
    //     blockKeyboardProcess(getRunningPID());
    //     _stint20();
    //     return buffer;
    // } else {
    //     /* read from fd */
    //     
    // }
    
}

void writeByteFD(int fd, char c) {
    if (fd_table[fd].pos == FD_SIZE) {
        blockProcess(getRunningPID());
        _stint20();
    }
    if (fd == 1 && c!= -1) {
        drawChar(c, 0xFFFFFF, 0x000000);
    }
    fd_table[fd].fdbuff[fd_table[fd].pos] = c;
    fd_table[fd].pos++;
    return;
}


/**
 * @brief Espera a que el caracter ingresado por entrada de teclado sea un salto de línea.
 */
void waitForEnter(){
	unsigned char c;
	while((c = read(0)) != '\n');
}

void strcpy(char * dest, char * src, int destSize){
    int i = 0;
    while(src[i] != '\0' && i < destSize-1){
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int strcmplen(char * str1, char * str2, int len){
	int i = 0;
	while(str1[i] != '\0' && str2[i] != '\0' && i < len){
		if (str1[i] != str2[i]){
			return 1;
		}
		i++;
	}
	return 0;
}
