all:  serverAoutput serverBoutput serverCoutput awsoutput client monitoroutput 

#make the serverA	
serverAoutput:  serverA.c 
	gcc -o serverAoutput serverA.c

serverBoutput:  serverB.c 
	gcc -o serverBoutput serverB.c

serverCoutput:  serverC.c 
	gcc -o serverCoutput serverC.c -lm

awsoutput:  aws.c 
	gcc -o awsoutput aws.c

client:  client.c
	gcc -o client client.c

monitoroutput: monitor.c
	gcc -o monitoroutput monitor.c

serverA:
	./serverAoutput

serverB:
	./serverBoutput	

serverC:
	./serverCoutput

aws:
	./awsoutput

monitor:
	./monitoroutput	
