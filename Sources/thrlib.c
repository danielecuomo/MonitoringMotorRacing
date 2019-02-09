#include <stdio.h>			
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>	

#include "thrlib.h"

pthread_mutex_t wr_race = PTHREAD_MUTEX_INITIALIZER;	/* mutex on fd_exit */

pthread_mutex_t m_list = PTHREAD_MUTEX_INITIALIZER;		/* mutex on L_cars */

pthread_mutex_t inc_tot_th = PTHREAD_MUTEX_INITIALIZER; /* mutex on tot_thread */

void *init_sr( void *tid_cl ){
	car *car_tmp;
	pthread_t tid_car;
	int res;
	char c_arr[50];
	if( LEADER )
		signal( SIGUSR1 , letsgo );
	while( !START ){
		car_tmp = (car *)malloc(sizeof(car));
		if( read( fd_curr , car_tmp , sizeof(car) ) == -1 )
			{ perror("Read failed"); exit(-1); }
			
		if( car_tmp->ID == SAFETY_CAR.ID ){
			if( pthread_cancel( *(pthread_t *)tid_cl ) )
				{ perror("pthread_cancel failed"); exit(-1); }
			if( LEADER || (argv2 == 's') || (argv2 == 'S') ){
				if( write( STDOUT_FILENO , "\nEnd car accepting\n" , strlen("\nEnd car accepting\n\n") ) == -1 )
					{ perror("Write failed"); exit(-1); }
			}
			if( !LEADER ){
				if( write( fd_exit , &SAFETY_CAR , sizeof(car) ) == -1 )
					{ perror("Write failed"); exit(-1); }
			}
			START = true;
			send_to_all( L_cars );
		}
		else{
			if( LEADER ){
				sprintf( c_arr , "Car %d is ready to start!\n" , car_tmp->ID );
				if( write( STDOUT_FILENO , c_arr , strlen(c_arr) ) == -1 )
					{ perror("Write failed"); exit(-1); }
			}
			
			if( pthread_mutex_lock(&m_list) )
					{ perror("pthread_mutex_lock failed"); exit(-1); }
			res = search_tid ( car_tmp->ID , L_cars );
			if( pthread_mutex_unlock(&m_list) )
					{ perror("pthread_mutex_lock failed"); exit(-1); }
			
			if ( !res ){
				if( pthread_create( &tid_car , NULL , init_car , (void *)car_tmp ) )
					{ perror("pthread_create failed"); exit(-1); }
			}
		}
	}
}

void *init_cl( void *NIL ){
	int fd_cl;
	car *my_car;
	pthread_t tid_car;
	if( LEADER || (argv2 == 's') || (argv2 == 'S') ){
		if( write( STDOUT_FILENO , "\nStart car accepting\n\n" , strlen("\nStart car accepting\n\n") ) == -1 )
			{ perror("Write failed"); exit(-1); }
	}
	while( 1 ){
		if( (fd_cl = accept(fd_entry , NULL, NULL)) == -1 )
			{ perror("Accept failed"); exit(-1); }
		my_car = (car *)malloc(sizeof(car));
		if( read(fd_cl, my_car, sizeof(car)) == -1 )
			{ perror("Read failed"); exit(-1); }
		if( pthread_create( &tid_car , NULL , init_car , (void *)my_car ) )
			{ perror("pthread_create failed"); exit(-1); }
		if( write( fd_cl , "1" , 1 ) == -1 )
			{ perror("Write failed"); exit(-1); }
		close(fd_cl);
	}
}

void *init_car(void *car_tmp){
	car *my_car = (car *)car_tmp;
	
	if( pthread_mutex_lock( &m_list ) )
		{ perror("pthread_mutex_lock failed"); exit(-1); }
	L_cars = push( L_cars , my_car->ID , pthread_self( ) );
	if( pthread_mutex_unlock( &m_list ) )
		{ perror("pthread_mutex_unlock failed"); exit(-1); }

	if( pthread_mutex_lock( &inc_tot_th ) )
		{ perror("pthread_mutex_lock failed"); exit(-1); }
	tot_thread++;
	if( pthread_mutex_unlock( &inc_tot_th ) )
		{ perror("pthread_mutex_unlock failed"); exit(-1); }

	if( write(fd_exit, my_car, sizeof(car)) == -1 )
		{ perror("Write failed"); exit(-1); }

	pause( );
	
	/*------------------------ Race control ------------------------*/
	if( LEADER ){
		if( pthread_mutex_lock( &wr_race ) )
			{ perror("pthread_mutex_lock failed"); exit(-1); }
		if( write(fd_exit, my_car, sizeof(car)) == -1 )
			{ perror("Write failed"); exit(-1); }
		if( pthread_mutex_unlock( &wr_race ) )
			{ perror("pthread_mutex_unlock failed"); exit(-1); }
	}
	unsigned int sec;
    while( !FINISH ){
		pause( );
		sec = my_car->time + rand()%my_car->time;
		sleep( sec );
		
		if( pthread_mutex_lock( &wr_race ) )
			{ perror("pthread_mutex_lock failed"); exit(-1); }
		if( write(fd_exit, my_car, sizeof(car)) == -1 )
			{ perror("Write failed"); exit(-1); }
		if( pthread_mutex_unlock( &wr_race ) )
			{ perror("pthread_mutex_unlock failed"); exit(-1); }
	}
	/*--------------------------------------------------------------*/
	free( my_car );
}

void letsgo( int num_sig ){
	if( write( fd_exit , &SAFETY_CAR , sizeof(car) ) == -1 )
		{ perror("Write failed"); exit(-1); }
}

void do_nothing( int num_sig ){}
