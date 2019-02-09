#ifndef THRLIB_H
#define THRLIB_H

#include "listlib.h"

/*-------------- THREADS LIBRARY ---------------*/

typedef struct s_car{
	int ID;
	unsigned int time;
}car;

bool LEADER, FINISH, START;								/* states of race */

int tot_thread;											/* number of init_car threads */

char argv2;												/* used to save argv[2] */

car SAFETY_CAR;											/* end accepting phase and race control */

int fd_entry, fd_exit, fd_curr;  						/* fd_curr: File descriptor used by accept
														 * fd_exit: File descriptor for next server
														 * fd_entry: File descriptor for create fd_curr */
														
car_t *L_cars;											/* list for tid of init_car */

void *init_sr( void * );
/* accepts cars from previous server */
void *init_cl( void * );
/* accepts cars from clients */
void *init_car( void * );
/* one thread related to one car */
/*------------------------------------------*/

/*----------------- HANDLER ----------------*/
void letsgo( int );										/* sends SAFETY_CAR to next server */
void do_nothing( int );									/* ignore a signal */
/*------------------------------------------*/

#endif
