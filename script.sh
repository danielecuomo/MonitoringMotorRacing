#!/bin/bash
# This script takes 6 parameters:
#	-port number of first server
#	-number of servers
#	-number of laps
#	-parameter ['S' / 'N'] for cars auto insert
# 	-path of server syntax: <pathname>/./<server>
#	-path of client syntax: <pathname>/./<client>

# argv[2] value is i%2 -> { S if 0 , N if 1 }

for((i=0;i<$2;i++)); do
	if [ $(($i%2)) -ne 0 ]; then
		`gnome-terminal -x $5 $(($1+$i)) s 127.0.0.1 $(($1+(($i+1)%$2))) $3`
	else
		`gnome-terminal -x $5 $(($1+$i)) n 127.0.0.1 $(($1+(($i+1)%$2))) $3`
	fi
`sleep 1` 	
done

`sleep 3`

if [ $4 = S ] || [ $4 = s ]; then
	n=$((5 + $RANDOM%11))
	for((i=0;i<n;i++)); do
		sec=$(($RANDOM%4 + 1))
		$6 $(($i+1)) 127.0.0.1 $(($1+($i%$2))) $sec
	`sleep 1`
	done
fi
