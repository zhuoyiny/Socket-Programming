//
//  client.c
//
//
//  Created by Zhuoying Yi on 11/13/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdbool.h>

#define AWSPORT "25105"  //aws TCP port
#define HOST "localhost"
#define MAXDATASIZE 100 // max number of bytes can get

// modified from Beej's code
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd = 0;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char *clientInput[3];
    int linkid;
    int size;
    int power;
    double delay;

    if (argc != 4) {
        fprintf(stderr,"usage: client hostname\n");
        return 1;
    } else {
        for(int i = 1; i < 4; i++){
            clientInput[i-1] = (argv[i]);
        }
        linkid = atoi(argv[1]);
        size = atoi(argv[2]);
        power = atoi(argv[3]);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(HOST, AWSPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
//          perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    freeaddrinfo(servinfo);
    printf("The client is up and running\n");

    if(send(sockfd, &linkid, sizeof(int),0) == -1){
        perror("send");
        exit(1);
    }
    if(send(sockfd, &size, sizeof(int),0) == -1){
        perror("send");
        exit(1);
    }
    if(send(sockfd, &power, sizeof(int),0) == -1){
        perror("send");
        exit(1);
    }

    printf("The client sent ID=<%d>, size=<%d>, and power=<%d> to AWS\n",linkid, size, power);

    int nomatch;
    if ((recv(sockfd, &nomatch, sizeof(int), 0)) == -1) {
        perror("recv");
        exit(1);
    }
    
    if(nomatch == 1) {
        printf("Found no matches for link <%d>\n",linkid);
    }
    if(nomatch == 0) {
        if ((recv(sockfd, &delay, sizeof(double), 0)) == -1) {
            perror("recv");
            exit(1);
        }
        printf("The delay for link <%d> is <%.2f>ms\n",linkid, delay);
    }
    
    close(sockfd);

    return 0;
}
