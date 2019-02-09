# MonitoringMotorRacing

## Quick Start
gcc server.c listlib.c ranklib.c thrlib.c -o $1 -lpthread

## The project
A multi-threaded client / server application that simulates the monitoring system
or journey times for motor racing. The system consists of a sequence of n detection stations.
The system consists of three distinct phases.
### Creating the circuit
The server-station processes are linked together to create a
communication ring
### Cars entry
The client-car processes connect to the circuit and all come to one
same server-station, called server-departure, identified as the server with identifier
lowest among all server-stations.
### Race control
 The 'server-departure' starts the race. Each station forwards the cars to the
next station. At the end of each lap the start station displays the ranking
temporary. After a fixed number of laps, the race ends.
The application is written using the C language. The communication between the processes occurs through TCP sockets.

### Circuit Creation
 In a first phase, the stations connect to each other to create a communication circuit. Assume that each station receives the connection request exactly from another station and that the requests create a 'circuit', i.e., a ring of monitoring stations.
### Cars entry
Once the first phase is over, the stations can receive requests for connection from the cars. A car connects to a single monitoring station.
Each station can receive requests for connections from multiple cars. At the time of connection, the vehicle sends to the station an ID, ID, and an integer, TIME, which indicates the minimum number of seconds that the car uses to move from one station to the next.
When the server receives a connection request from a vehicle, a new car management thread is created and informs the other server-stations.
Each server-station creates a different thread for each car on the circuit. All communications between a server-station and the next in the ring occur through a
single socket.
All threads of a station send data to the corresponding threads of the next station. Every
server-station maintains the ranking of the transited vehicles locally. For each car the identification is memorized, and the time elapsed between the start of the race and the moment in which the vehicle passed through the point of detection.
All cars are made to 'pass' (along the station ring) to the starting station.
Race: The starting station, after receiving the SIGUSR1 signal, starts the race.
In each station, each thread:

• Wait for your vehicle to arrive

• Update the local ranking

• It waits a time interval equal to TIME + X where X is chosen randomly in the interval [0, TIME].

• Forward the car to the corresponding thread in the next station.

If the interaction level is 'S', the current station displays the current ranking. The ranking is displayed when all the cars have passed through the current station for the current lap.
The start station displays the rankings on each lap regardless of the level value
interaction.
