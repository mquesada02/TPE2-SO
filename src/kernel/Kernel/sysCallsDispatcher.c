#include <stdint.h>
#include <interrupts.h>
#include <videodriver.h>
#include <speakerDriver.h>
#include <keyboardDriver.h>
#include <time.h>
#include <clock.h>
#include <lib.h>
#include <MemoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <pipe.h>
//#include <semaphore.h>


extern char buffer;
extern long int registers_space[];





/**
 * @brief Función que espera al transcurso de los segundos deseados que se reciben como parámetro.
 * 
 * @param seconds Segundos que se desean esperar.
 */
void wait(int seconds){
    int initial = ticks_elapsed();
	while ( (double) (ticks_elapsed()-initial)/18.0 < seconds );
}

/**
 * @brief Llama a los distintos Handlers para cada interrupción de software.
 * 
 * @param syscall Número de la interrupción de software
 * @param param1 Primer parámetro recibido. Corresponde al registro RSI.
 * @param param2 Segundo parámetro recibido. Corresponde al registro RDX
 * @param param3 Tercer parámetro recibido. Corresponde al registro R10.
 * @param param4 Cuarto parámetro recibido. Corresponde al registro R8.
 * @param param5 Quinto parámetro recibido. Corresponde al registro R9.
 * @return Retorna en el caso de ser necesario, el valor de retorno de la interrupción llamada. En otro caso
 * retorna el valor 0.
 */
long int syscallsDispatcher (uint64_t syscall, long int param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5) {
    switch (syscall) {
		case 0:
			return read(/* param1*/ getSTDIN(getRunningPID()) );
        case 1:
            if (param1==127)    
                deleteChar();
            else
                write(getSTDOUT(getRunningPID()), param1, param2, param3);
            break;
        case 2:
            drawNextLine();
            break;
        case 3:
        	_sti();
            wait(param1);
            break;
        case 4:
            timeToStr((char*)param1);
            break;     
        case 5: 
            if (getAltTouched()){
                return (intptr_t)registers_space;      
            }
            return 0;
        case 6:
            beep(param1,param2);
            break;
        case 7:
            drawCircle(param1, param2, param3, param4);
            break;
        case 8:
            drawRectangle(param1, param2, param3, param4, param5);
            break;
        case 9:
            loadScreen();         
            break;
        case 10:
            enableDoubleBuffer(param1);            
            break;
        case 11:
            getKeyboardState((char*)param1);
            break;
        case 12:
            drawCharAt(param1, param4, param5, param2, param3);            
            break;
        case 13:
            getMemStatus(param1, param2);
            break;
        case 14:
            return allocMemory(param1);
            break;
        case 15:
            return freeMemory(param1);
            break;
        case 16:
            struct processStart {
                char foreground;
                char * name;
            };
            int value = startProcess(param1, param2, param3, param4, ((struct processStart *)param5)->foreground, ((struct processStart *)param5)->name);
            if(((struct processStart *)param5)->foreground) //foreground
                setProcessState(getRunningPID(), blocked);
            _stint20();
            return value;
            break;
        case 17:
            exitProcess();
            break;
        case 18:
            return getRunningPID();
            break;
        case 19:
            return killProcess(param1);
            break;
        case 20:
            printProcesses();
            break;
        case 21:
            return switchBlock(param1);
            break;
        case 22:
            waitPID(param1);
            break;
        case 23:
            return changePriority(param1, param2);
            break;
        case 24:
            return sem_open(param1, param2);
            break;
        case 25:
            return sem_close(param1);
            break;
        case 26:
            return sem_wait(param1);
            break;
        case 27:
            return sem_post(param1);
            break;
        case 28:
            setSTDIN(param1, param2);
            break;
        case 29:
            setSTDOUT(param1, param2);
            break;
        case 30:
            openFD(param1, param2);
            break;
        case 31:
            closeFD(param1, param2);
            break;
        case 32:
            return pipe(param1);
            break;
        case 33: 
            return getSTDIN(getRunningPID());
            break;
        case 34:
            return getSTDOUT(getRunningPID());
            break;
        case 35:
            struct processStartSTD {
                char foreground;
                char * name;
                char stdin;
                char stdout;
            };
            return pipeProcess(param1, param2, param3, param4, ((struct processStartSTD *)param5)->foreground, ((struct processStartSTD *)param5)->name, ((struct processStartSTD *)param5)->stdin, ((struct processStartSTD *)param5)->stdout);

	}
	return 0;
}
