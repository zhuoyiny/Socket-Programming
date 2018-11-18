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

#define AWSPORT "26105"  //aws TCP port with monitor
#define HOST "localhost"
#define MAXDATASIZE 100

// modified from Beej's code
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd = 0;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int linkid;
    int size;
    int power;
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
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    freeaddrinfo(servinfo);
    printf("The monitor is up and running\n");

    while(1) {
        //recv the three input parameters from aws
        if ((recv(sockfd, &linkid, sizeof(int), 0)) == -1) {
            perror("recv");
            exit(1);
        }
        if ((recv(sockfd, &size, sizeof(int), 0)) == -1) {
            perror("recv");
            exit(1);
        }
        if ((recv(sockfd, &power, sizeof(int), 0)) == -1) {
            perror("recv");
            exit(1);
        }
        printf("The monitor received link ID=<%d>, size=<%d>, and power=<%d> from the AWS\n", linkid, size, power);

        double transdelay;
        double propdelay;
        double endtoend;

        int nomatch;
        if ((recv(sockfd, &nomatch, sizeof(int), 0)) == -1) {
            perror("recv");
            exit(1);
        }

        if(nomatch == 1) {
            printf("Found no matches for link <%d>\n",linkid);
        }
        if(nomatch == 0)
        {
            if ((recv(sockfd, &transdelay, sizeof(double), 0)) == -1) {
                perror("recv");
                exit(1);
            }
            if ((recv(sockfd, &propdelay, sizeof(double), 0)) == -1) {
                perror("recv");
                exit(1);
            }
            if ((recv(sockfd, &endtoend, sizeof(double), 0)) == -1) {
                perror("recv");
                exit(1);
            }
            printf("The result for link <%d>:\nTt = <%.2f>ms\nTp = <%.2f>ms\nDelay = <%.2f>ms\n",linkid, transdelay, propdelay, endtoend);

        }


    }
    close(sockfd);

    return 0;
}
