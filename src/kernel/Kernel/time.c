// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <interrupts.h>

static unsigned long ticks = 0;

/**
 * @brief Handler para la interrupción del Timer Tick. Incrementa constantemente el valor de ticks.
 */
void timer_handler() {
	
	//if (ticks % 18 == 0)
	//	_timeHandler();

	ticks++;
}

/**
 * @brief Retorna la cantidad de ticks transcurridos.
 * 
 * @return Cantidad de ticks transcurridos.
 */
int ticks_elapsed() {
	return ticks;
}

/**
 * @brief Retorna la cantidad de segundos transcurridos a partir de los ticks.
 * 
 * @return Cantidad de segundos transcurridos.
 */
int seconds_elapsed() {
	return ticks / 18;
}

