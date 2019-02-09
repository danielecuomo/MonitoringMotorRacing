#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "listlib.h"

car_t *push( car_t *L , int id , pthread_t T ){
	car_t *L_car = (car_t *)malloc(sizeof(car_t));
	L_car->ID = id;
	L_car->tid = T;
	L_car->next = L;
	return L_car;
}

pthread_t search_tid( int id , car_t *L ){
	if( L ){
		if( L->ID == id )
			return L->tid;
		else
			return search_tid( id , L->next );
	}
	return 0;
}

void delete_list( car_t *L ){
	if( L ){
		delete_list( L->next );
		free( L );
	}
}

void send_to_all( car_t *L ){
	if( L ){
		pthread_kill( L->tid , SIGUSR2 );
		send_to_all( L->next );
	}
}
