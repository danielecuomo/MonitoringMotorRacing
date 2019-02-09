#include <arpa/inet.h> 		/* "htonl", "htons", "inet_aton" */
#include <sys/socket.h> 	/* "socket", "connect", "inet_aton" */
#include <stdio.h> 			/* "sprintf", "perror" */
#include <netinet/in.h>		/* define sockaddr_in, "inet_aton" */
#include <stdlib.h>			/* "malloc" */
#include <string.h>			/* "strlen" */
#include <unistd.h>			/* STDERR_FILENO */

typedef struct s_car{
	int ID;
	unsigned int time;
}car;

int main( int argc , char *argv[] ){
	int id = atoi( argv[1] );
	if( id < 1 ){
		if( write( STDERR_FILENO , "error: car value not accepted\n" , strlen("error: car value not accepted\n") ) == -1 )
			{ perror("Write failed"), exit(-1); }
		exit( -1 );
	}
		
	car my_car;
	int fd_car;
    struct sockaddr_in cl_car;
    cl_car.sin_family = AF_INET;				/*Used for protocol TCP*/
    cl_car.sin_port = htons( atoi(argv[3]) );
    inet_aton( argv[2] , &cl_car.sin_addr );
    
    if( (fd_car = socket(PF_INET, SOCK_STREAM, 0)) == -1 )
		{ perror("Socket creation failed for fd_car"), exit(-1); }
    if( connect(fd_car, (struct sockaddr *)&cl_car, sizeof(cl_car)) == -1 )
		{ perror("Failed to connect to server"), exit(-1); }

	my_car.ID = id;
	my_car.time = (unsigned int)atoi( argv[4] );
    if( write( fd_car, &my_car, sizeof(car) ) == -1 )
		{ perror("Write failed"), exit(-1); }
	
	char c;
	if( read( fd_car , &c , sizeof(char) ) == -1 )
		{ perror("Read failed"), exit(-1); }
	int lenght_end = 37 + strlen(argv[1]) + strlen(argv[2]) + strlen(argv[3]);
    char end_str[lenght_end];
    sprintf(end_str, "Car %s data sent correctly to server %s:%s\n", argv[1], argv[2], argv[3]);
    if( write(STDOUT_FILENO, end_str, lenght_end) == -1 )
		{ perror("Write failed"), exit(-1); }
    return 1;
}
