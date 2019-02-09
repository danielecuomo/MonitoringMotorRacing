#ifndef RANKLIB_H
#define RANKLIB_H

#include <time.h>

/*------------- RANKING LIBRARY -------------*/

int tot_thread;		/* number of car-threads */
int lap;			/* number of laps */

typedef struct list_rank{
	time_t *T;
	int *A;
	struct list_rank *next;
}st_rank;

st_rank *alloc_rank( void );
/* allocate struct of st_rank type */
st_rank *create_ranking( st_rank * , int , time_t );
/* build ranking of curring lap */
st_rank *pop_rank( st_rank * );
/* deallocate the head of stack */
void print_rank( st_rank * );
/* print on stdout current ranking */

#endif
