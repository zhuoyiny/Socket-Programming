# UNIX socket programming -- Computer Networks

The objective of this assignment is to familiarize with UNIX socket programming.

In this project, I implemented a model computational offloading where a single client issues three parameters (link ID, file/packet size, and signal power) to the AWS server and expects the reply for the end-to-end delay of the designated link.
![Illustration of the network.png](https://github.com/zhuoyiny/Socket-Programming/blob/master/Illustrationofthenetwork.png)
The setup of the network is illustrated as follows. The server communicating with the client acts similar as AWS (Amazon Web Server). There are three backend servers, named Back-Server A, Back-Server B and Back-Server C. Back-Servers A and B are storage servers. Each of them possesses a database file, database_a.csv and database_b.csv, respectively, in which attribute values regarding information of links are stored. A toy database is demonstrated in Table 1. There are five attributes in the database, which are {Link ID, Bandwidth, Length, Velocity (Propagation Speed), Noise Power}, from left to right. In database_a.csv and database_b.csv, there will be only five columns of numbers without names of attributes.

The Back-Server C is a computing server. It receives data from the AWS server, performs computational tasks, and returns the results to the AWS server. The monitor connecting to the AWS server is used to record results of every steps and print them out. The client, monitor and the AWS communicate over TCP connections while the AWS and the Back-Servers A, B, & C communicate over UDP connections.

The AWS server searches for the information of the designated link (identified by Link ID) over storage servers. After receiving replies from storage servers, the AWS server then sends the link information (bandwidth, link length, propagation velocity, noise power) with client inputs (link ID, file size, signal power) to the computing server. Once the AWS server receives the computed results (which will be end-to-end delay, propagation delay, and transmission delay) from server C, it finally communicates the end-to-end delay to the client in the required format (this is also an example of how a cloud-computing service such Amazon Web Services might speed up a large computation task offloaded by the client).


###### What have I done in this assignment:

the client gets three parameters from input command line, sending them to aws server using TCP. AWS gets the three parameters, sending them to monitor, and send link id to back-end server A and B. Server A and B get link id from AWS using UDP, searching in database a and b, sending back the result to AWS. If there is a match, the AWS server will send the input with the received link information to the back-end server C to calculate the transmission delay, propagation delay, and the end-to-end delay. After getting the calculated results from back-end server C, send the result back to client and monitor. During the whole process, client, monitor and AWS communicate over TCP connections. AWS and the Back-Servers A, B, C communicate over UDP connections.

###### What my code files are and what each one of them does:

- client.c: it gets the three parameters, sending them to aws server using TCP. Getting back the result and show on the screen after calculated.

- aws.c: it gets the three parameters from client.c, and send them to monitor, send link id to back-end server A and B to search if there is a match. If there is a match, the AWS server will send the input with the received link information to the back-end server C to calculate the transmission delay, propagation delay, and the end-to-end delay. After getting the calculated results from back-end server C, send the result back to client and monitor.

- serverA.c: it gets link id from AWS using UDP, it will search in database a and then send back the result to aws.

- serverB.c: it gets link id from AWS using UDP, it will search in database b and then send back the result to aws.

- serverC.c: it gets input by user and the received link information get from database from AWS using UDP, then making calculation and send back the result to aws.

- monitor.c: it gets the three parameters from AWS server and link information get from database A and B from AWS through TCP.
