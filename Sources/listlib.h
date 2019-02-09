#ifndef LISTLIB_H
#define LISTLIB_H

#include <pthread.h>

/*------------- LIST LIBRARY -------------*/

typedef struct thread_car{
	int ID;
	pthread_t tid;
	struct thread_car *next;
}car_t;

car_t *push( car_t * , int , pthread_t );
/* insert a struct car_t in list */
pthread_t search_tid( int , car_t * );
/* search tid in list */
void delete_list( car_t * );
/* deallocate a list */
void send_to_all( car_t * );
/* send SIGUSR2 to all init_car threads */

#endif
