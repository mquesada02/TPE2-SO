#include <library.h>
#include <semLibrary.h>

#define NULL (void*)0
#define N 25 /* número mäximo de filósofos */
#define LEFT (i+N-1)%N /* número del vecino izquierdo de i */
#define RIGHT (i+1)%N /* número del vecino derecho de i */
#define THINKING 0 /* el filósofo está pensando */
#define HUNGRY 1 /* el filósofo trata de obtener los tenedores */
#define EATING 2 /* el filósofo está comiendo */

typedef int sem_type; 

extern int state[N];
extern sem_type *mutex;
extern sem_type *s[N];
extern int last_i;

void think();

void eat();

void take_forks(int i);

void leave_forks(int i);

char* write_state();

