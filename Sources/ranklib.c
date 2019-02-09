#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "thrlib.h"
#include "ranklib.h"

st_rank *alloc_rank(){
	st_rank *L_rank = (st_rank *)malloc(sizeof(st_rank));
	L_rank->T = (time_t *)calloc( tot_thread , sizeof(time_t));
	L_rank->A = (int *)calloc(tot_thread , sizeof(int));
	L_rank->next = NULL;
	return L_rank;
}

st_rank *create_ranking( st_rank *L_rank , int id , time_t t ){
	if( L_rank ){
		int *curr = L_rank->A;
		time_t *T = L_rank->T;
		int i = 0;
		while( curr[i] && (i < tot_thread) ){
			if( curr[i] == id ){
				L_rank->next = create_ranking( L_rank->next , id , t );
				return L_rank;
			}
			else
				i++;
		}
		if( i < tot_thread ){
			curr[i] = id;
			T[i] = t;	
		}
		if( i+1 == tot_thread ){
			if( LEADER || (argv2 == 's') || (argv2 == 'S') )
				print_rank( L_rank );
			lap++;
			return pop_rank( L_rank );
		}
		return L_rank;
	}
	else{
		st_rank *L_tmp = alloc_rank();
		L_tmp->A[0] = id;
		L_tmp->T[0] = t;
		return L_tmp;
	}
}

st_rank *pop_rank( st_rank *st ){
	st_rank *tmp = st->next;
	free( st->A );
	free( st->T );
	free( st );
	return tmp;
}

void print_rank( st_rank *L ){
	char R[1000];
	int i;
	struct tm *t_curr;
	sprintf( R , "\nRanking - Lap: %d\n" , lap );
	for( i=0 ; i<tot_thread ; i++ ){
		t_curr = localtime( &L->T[i] );
		sprintf( R , "%s %d. %d		%d:%d:%d\n" , R , i+1 , L->A[i], t_curr->tm_hour , t_curr->tm_min , t_curr->tm_sec );
	}
	if( write( STDOUT_FILENO , R , strlen(R) ) == -1 )
		{ perror("Write failed"); exit(-1); }
}
