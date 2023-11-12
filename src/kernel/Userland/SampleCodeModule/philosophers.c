/*SOLUCIÓN AL PROBLEMA DE LOS FILÓSOFOS COMENSALES DEL LIBRO DE TANENBAUM*/
#include <philosophers.h>

int state[N]; /* arreglo que lleva registro del estado de todos*/
sem_type *mutex = NULL; /* exclusión mutua para las regiones críticas */
sem_type *s[N] = {0}; /* un semáforo por filósofo */
int last_i = 0; /* indice del ultimo filosofo */
int philosopherPIDs[N];

void test(int i);

// void start_phil(int cant){
//     char aux[7] = {'p', 'h', 'i', 'l'};
//     for (int i = 0; i < cant; i++){
//         state[i] = THINKING;
//         if(i <= 9){
//             aux[4] = i + '0';
//             aux[5] = '\0';
//         }
//         else {
//             aux[4] = (i / 10) + '0';
//             aux[5] = (i % 10) + '0';
//             aux[6] = '\0';
//         }
//         s[i] = sem_open(aux, 1);
//     }
//     mutex = sem_open("mutex_phil", 1);
//     last_i = cant;
// }

void think(int i){
    //printf("Th%d\n", i);
    sleep(3);
}

void eat(){
    sleep(1);
}

void take_forks(int i) /* i: número de filósofo, de 0 a N1 */
{
    sem_wait(mutex); /* entra a la región crítica */
    state[i] = HUNGRY; /* registra el hecho de que el filósofo i está hambriento */
    test(i); /* trata de adquirir 2 tenedores */
    sem_post(mutex); /* sale de la región crítica */
    sem_wait(s[i]); /* se bloquea si no se adquirieron los tenedores */
}

void leave_forks(int i) /* i: número de filósofo, de 0 a N–1 */
{
    sem_wait(mutex); /* entra a la región crítica */
    state[i] = THINKING; /* el filósofo terminó de comer */
    test(LEFT); /* verifica si el vecino izquierdo puede comer ahora */
    test(RIGHT); /* verifica si el vecino derecho puede comer ahora */
    sem_post(mutex); /* sale de la región crítica */
}

void test(int i) /* i: número de filósofo, de 0 a N1 */
{
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        write_state();
        sem_post(s[i]);
    }
}

char* write_state(){
    //printf("en write_state\n");
    char buffer[2*N+1];
    int i;
    for(i=0; i<last_i; i++){
        buffer[2*i] = state[i]==EATING? 'E' : state[i]==HUNGRY? 'H' : 'T';
        buffer[2*i+1] = ' ';
    }
    buffer[2*i] = '\0';
    printf("%s\n", buffer);
}