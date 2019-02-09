#include <arpa/inet.h> 		
#include <sys/socket.h> 	
#include <stdio.h> 			
#include <netinet/in.h>		
#include <stdlib.h>			
#include <string.h>			
#include <pthread.h>		
#include <time.h>			
#include <signal.h> 		
#include <unistd.h>			
#include <stdbool.h>	

#include "ranklib.h"		
#include "listlib.h"		
#include "thrlib.h"			

int main( int argc , char *argv[] ){
	
    srand(time(NULL));									/* Used for select a pseudo-random ID */
    argv2 = *argv[2];
    L_cars = NULL;
    SAFETY_CAR.ID = -1;
    SAFETY_CAR.time = 0;
    
    /*-------------- Creation of connection for fd_entry--------------*/
    struct sockaddr_in station_in;						/* Socket used to accept cars from outside and the previous server */
    station_in.sin_family = AF_INET;			       	/* AF_INET used by TCP protocol */
    station_in.sin_port = htons( atoi(argv[1]) );
    station_in.sin_addr.s_addr = htonl( INADDR_ANY ); 
    
    fd_entry = socket(PF_INET, SOCK_STREAM, 0);			/* PF_INET = network socket
														 * SOCK_STREAM = used by local or TCP connection */
    if (fd_entry == -1)
		{ perror ("Socket creation failed for fd_entry"); exit(-1); }
    if (bind(fd_entry, (struct sockaddr *)&station_in, sizeof(station_in)) == -1)
		{ perror ("Bind failed for fd_entry"); exit(-1); }
    if (listen(fd_entry, 10) == -1)
		{ perror ("Listen failed for fd_entry"); exit(-1); }
    /*---------------------------------------------------------------*/
    
    /*-------------- Creation of connection for fd_exit--------------*/
    struct sockaddr_in station_out;						/* Socket used to send cars to server */

    station_out.sin_family = AF_INET;					/* AF_INET used by TCP protocol */
    station_out.sin_port = htons( atoi(argv[4]) );
    
    inet_aton(argv[3], &station_out.sin_addr);		 	/* PF_INET = network socket
														 * SOCK_STREAM = used by local or TCP connection */
    if ((fd_exit = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		{ perror ("Socket creation failed for fd_exit"); exit(-1); }
		
    if (write(STDOUT_FILENO, "Connecting...\n", strlen("Connecting...\n")) == -1)
		{ perror("Write failed"); exit(-1); }
    
    while (connect(fd_exit, (struct sockaddr *) &station_out, sizeof(station_out)) == -1);
														/* Waits until next server is available for a connection */
    
    if( write(STDOUT_FILENO, "Connected\n\n", strlen("Connected\n\n")) == -1 )
		{ perror("Write failed"); exit(-1); }
    if((fd_curr = accept(fd_entry, NULL, NULL)) == -1)	/* Accepts a connection from the previous server */
		{ perror("Accept failed"); exit(-1); }
		
    /*---------------------------------------------------------------*/
    
    /*------------- Ring connection and Leader election -------------*/
    int ID_pred, min, zero = 0, my_ID = rand();			/* ID_pred   -> ID passed by previous server
														 * my_ID 	 -> Current server ID
														 * min 		 -> Minimum ID (Leader Election)
														 * zero 	 -> Send "0" to next server when ring is completed and leader is selected*/
    bool ck_ring;										/* flag used to check the ring*/
    
    if( write(fd_exit, &my_ID, sizeof(int)) == -1 )
		{ perror("Write failed"); exit(-1); }
    if( read(fd_curr, &ID_pred, sizeof(car)) == -1)
		{ perror("Read failed"); exit(-1); }
	
    while( !ck_ring ){
        if( !ID_pred ){
            if( write(fd_exit, &zero, sizeof(int)) == -1 )
				{ perror("Write failed"); exit(-1); }
            ck_ring = true;
        }
        else if( my_ID != ID_pred ){
			if( my_ID < ID_pred )
				min = my_ID;
			else
				min = ID_pred;
			
            if( write(fd_exit, &min, sizeof(int)) == -1 )
				{ perror("Write failed"); exit(-1); }
            if( read( fd_curr, &ID_pred, sizeof(int)) == -1 )
				{ perror("Read failed"); exit(-1); }
        }
        else if( ID_pred == my_ID ){
            if( write(fd_exit, &zero, sizeof(int)) == -1 )
				{ perror("Write failed"); exit(-1); }
            while ( ID_pred ){
                if( read(fd_curr, &ID_pred, sizeof(int)) == -1 )
					{ perror("Read failed"); exit(-1); }
            }
            if( write(STDOUT_FILENO, "START - ", strlen("START - ") ) == -1 )
				{ perror("Write failed"); exit(-1); }
            LEADER = true;
            ck_ring = true;
			char c_lead[20];
			pid_t i_lead = getpid();
			sprintf( c_lead ,"PID LEADER: %d\n\n", i_lead );
			write(STDOUT_FILENO , c_lead , strlen(c_lead) );
        }
    }
    /*Track completed*/
    if( LEADER || (*argv[2] == 's') || (*argv[2] == 'S') ){
		if( write( STDOUT_FILENO , "Track completed\n" , strlen("Track completed") ) == -1 )
			{ perror("Write failed"); exit(-1); }
	}
	/*--------------------------------------------------------------*/
	
    /*---------------------- Car organization ----------------------*/
    int *fd_car;
	pthread_t tid_cl, tid_sr;
	
	tot_thread = 0;
	lap = 1;
	
	signal( SIGUSR2 , do_nothing );
	
	if( pthread_create( &tid_cl , NULL , init_cl , NULL ) )
		{ perror("pthread_create failed"); exit(-1); }
	if( pthread_create( &tid_sr , NULL , init_sr , (void *)&tid_cl ) )
		{ perror("pthread_create failed"); exit(-1); }
	if( pthread_join( tid_cl , NULL ) )
		{ perror("pthread_join failed"); exit(-1); }
	if( pthread_join( tid_sr , NULL ) )
		{ perror("pthread_join failed"); exit(-1); }
    /*--------------------------------------------------------------*/
    
    /*------------------------ Race control ------------------------*/
    int n_laps = atoi( argv[5] );
    pthread_t tid;
    car *car_tmp;
    st_rank *L_rank = alloc_rank( );
    
    while( lap <= n_laps ){
		car_tmp = (car *)malloc(sizeof(car));
		if( read(fd_curr, car_tmp, sizeof(car)) == -1 )
			{ perror("Read failed"); exit(-1); }
		
		L_rank = create_ranking( L_rank , car_tmp->ID , time(NULL) );

		tid = search_tid( car_tmp->ID , L_cars );
		if( pthread_kill( tid , SIGUSR2 ) )
			{ perror("pthread_kill failed"); exit(-1); }
	}
	/*--------------------------- End race -------------------------*/
	FINISH = true;
    char c;
    
	if( LEADER || (*argv[2] == 's') || (*argv[2] == 'S') ){
		if( write( STDOUT_FILENO , "\nRace finished!\n" , strlen("\nRace finished!\n") ) == -1 )
			{ perror("Write failed"); exit(-1); }
	}
	if( write( STDOUT_FILENO , "\nPress a key to continue..." , strlen("\nPress a key to continue...") ) == -1 )
		{ perror("Write failed"); exit(-1); }
    if( read( STDIN_FILENO , &c , 1 ) == -1 )
		{ perror("Read failed"); exit(-1); }
	close( fd_entry );
    close( fd_curr );
    close( fd_exit );
    delete_list( L_cars );
    /*--------------------------------------------------------------*/
}
