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

#define SERVERA "21105"
#define SERVERB "22105"
#define SERVERC "23105"
#define UDPPORT "24105"
#define TCPPORTCLIENT "25105"
#define TCPPORTMONITOR "26105"
#define BACKLOG 10


// modified from Beej's code
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

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
    int sockfd_tcp = 0, new_fd_tcp, sockfd_tcpM = 0, new_fd_tcpM;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints_tcp, *servinfo_tcp, *p_tcp, hints_tcpM, *servinfo_tcpM, *p_tcpM;
    struct sockaddr_storage their_addr_tcp, their_addr_tcpM, their_udp_addr, their_udp_addrB, their_udp_addrC; // connector's address information
    socklen_t sin_size_tcp, sin_size_tcpM;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv_tcp, rv_tcpM;
    socklen_t addr_len, addr_lenB, addr_lenC;
    int flag;
    int linkid;
    float bandwidth;
    float length;
    float velocity;
    float noisepower;

    int flagB;
    int linkidB;
    float bandwidthB;
    float lengthB;
    float velocityB;
    float noisepowerB;
    int size;
    int power;

    memset(&hints_tcp, 0, sizeof hints_tcp);
    hints_tcp.ai_family = AF_UNSPEC;
    hints_tcp.ai_socktype = SOCK_STREAM;
    hints_tcp.ai_flags = AI_PASSIVE;

    if ((rv_tcp = getaddrinfo("127.0.0.1", TCPPORTCLIENT, &hints_tcp, &servinfo_tcp)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_tcp));
        return 1;
    }

    for(p_tcp = servinfo_tcp; p_tcp != NULL; p_tcp = p_tcp->ai_next) {
        if ((sockfd_tcp = socket(p_tcp->ai_family, p_tcp->ai_socktype, p_tcp->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd_tcp, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd_tcp, p_tcp->ai_addr, p_tcp->ai_addrlen) == -1) {
            close(sockfd_tcp);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p_tcp == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo_tcp);

    if (listen(sockfd_tcp, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    printf("The AWS is up and running.\n");


    memset(&hints_tcpM, 0, sizeof hints_tcpM);
    hints_tcpM.ai_family = AF_UNSPEC;
    hints_tcpM.ai_socktype = SOCK_STREAM;
    hints_tcpM.ai_flags = AI_PASSIVE;

    if ((rv_tcpM = getaddrinfo("127.0.0.1", TCPPORTMONITOR, &hints_tcpM, &servinfo_tcpM)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_tcpM));
        return 1;
    }

    for(p_tcpM = servinfo_tcpM; p_tcpM != NULL; p_tcpM = p_tcpM->ai_next) {
        if ((sockfd_tcpM = socket(p_tcpM->ai_family, p_tcpM->ai_socktype, p_tcpM->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd_tcpM, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd_tcpM, p_tcpM->ai_addr, p_tcpM->ai_addrlen) == -1) {
            close(sockfd_tcpM);
            perror("server: bind");
            continue;
        }
        break;
    }

    if (p_tcpM == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo_tcpM);

    if (listen(sockfd_tcpM, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sin_size_tcpM = sizeof their_addr_tcpM;
    new_fd_tcpM = accept(sockfd_tcpM, (struct sockaddr *)&their_addr_tcpM, &sin_size_tcpM);
    if (new_fd_tcpM == -1) {
        perror("accept");
        exit(1);
    }
    inet_ntop(their_addr_tcpM.ss_family,
              get_in_addr((struct sockaddr *)&their_addr_tcpM),
              s, sizeof s);



    // modified from Beej's code
    int sockfd_A = 0;
  	struct addrinfo hints_A, *servinfo_A, *p_A;
  	int rv_A;
  	memset(&hints_A, 0, sizeof hints_A);
  	hints_A.ai_family = AF_UNSPEC;
  	hints_A.ai_socktype = SOCK_DGRAM;
  	if ((rv_A = getaddrinfo("127.0.0.1", SERVERA, &hints_A, &servinfo_A)) != 0) {
  		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_A));
  		return 1;
  	}
  	for(p_A = servinfo_A; p_A != NULL; p_A = p_A->ai_next) {
  		if ((sockfd_A= socket(p_A->ai_family, p_A->ai_socktype,
  				p_A->ai_protocol)) == -1) {
  			perror("talker: socket");
  			continue;
  		}
  		break;
  	}
  	p_A = servinfo_A;
  	if (p_A == NULL) {
  		fprintf(stderr, "talker: failed to create socket\n");
  		return 2;
  	}

    // modified from Beej's code
    int sockfd_B = 0;
       struct addrinfo hints_B, *servinfo_B, *p_B;
       int rv_B;
       memset(&hints_B, 0, sizeof hints_B);
       hints_B.ai_family = AF_UNSPEC;
       hints_B.ai_socktype = SOCK_DGRAM;

       if ((rv_B = getaddrinfo("127.0.0.1", SERVERB, &hints_B, &servinfo_B)) != 0) {
           fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_B));
           return 1;
       }
       for(p_B = servinfo_B; p_B != NULL; p_B = p_B->ai_next) {
           if ((sockfd_B= socket(p_B->ai_family, p_B->ai_socktype,
                   p_B->ai_protocol)) == -1) {
               perror("talker: socket");
               continue;
           }
           break;
       }
       if (p_B == NULL) {
           fprintf(stderr, "talker: failed to create socket\n");
           return 2;
       }

    // modified from Beej's code
    int sockfd_C = 0;
    struct addrinfo hints_C, *servinfo_C, *p_C;
    int rv_C;
    memset(&hints_C, 0, sizeof hints_C);
    hints_C.ai_family = AF_UNSPEC;
    hints_C.ai_socktype = SOCK_DGRAM;
    if ((rv_C = getaddrinfo("127.0.0.1", SERVERC, &hints_C, &servinfo_C)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_C));
        return 1;
    }
    for(p_C = servinfo_C; p_C != NULL; p_C = p_C->ai_next) {
        if ((sockfd_C= socket(p_C->ai_family, p_C->ai_socktype,
                              p_C->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }
    p_C = servinfo_C;
    if (p_C == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }


    while(1) {

        sin_size_tcp = sizeof their_addr_tcp;
        new_fd_tcp = accept(sockfd_tcp, (struct sockaddr *)&their_addr_tcp, &sin_size_tcp);
        if (new_fd_tcp == -1) {
            perror("accept");
            exit(1);
        }
        inet_ntop(their_addr_tcp.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr_tcp),
                  s, sizeof s);

        int nomatch = 0;

        if (!fork()) { // child process
            close(sockfd_tcp); // child doesn't need the listener

            if(recv(new_fd_tcp, &linkid,sizeof(int),0)==-1){
          	   perror("recieve");
          	}
            if(recv(new_fd_tcp,&size,sizeof(int),0)==-1){
          	   perror("recieve");
          	}
          	if(recv(new_fd_tcp,&power,sizeof(float),0)==-1){
          	   perror("recieve");
          	}

            printf("The AWS received ID=<%d>, size=<%d>, and power=<%d> from client using TCP over port 25105\n",linkid, size, power);

            if(send(new_fd_tcpM, &linkid, sizeof(int),0) == -1){
                perror("send");
                exit(1);
            }
            if(send(new_fd_tcpM, &size, sizeof(int),0) == -1){
                perror("send");
                exit(1);
            }
            if(send(new_fd_tcpM, &power, sizeof(int),0) == -1){
                perror("send");
                exit(1);
            }

            printf("The AWS sent link ID=<%d>, size=<%d>, and power=<%d> to the monitor using TCP over port <26105>\n",linkid, size, power);

            addr_len = sizeof their_udp_addr;// This is used for temporary saving Udp source destination
            //send x to serverA,B,C, and recev the caculated value from back-server
            if ((sendto(sockfd_A, &linkid, sizeof(char *), 0, p_A->ai_addr, p_A->ai_addrlen)) == -1) {
                perror("talker: send to A");
                exit(1);
            }
            printf("The AWS sent link ID=<%d> to Backend-Server A using UDP over port <24105>\n", linkid);



            addr_lenB = sizeof their_udp_addrB;// This is used for temporary saving Udp source destination
            //send x to serverA,B,C, and recev the caculated value from back-server
            if ((sendto(sockfd_B, &linkid, sizeof(char *), 0, p_B->ai_addr, p_B->ai_addrlen)) == -1) {
                perror("talker: send to B");
                exit(1);
            }
            printf("The AWS sent link ID=<%d> to Backend-Server B using UDP over port <24105>\n", linkid);


            if ((recvfrom(sockfd_A, &flag,sizeof(float) , 0, (struct sockaddr *)&their_udp_addr, &addr_len)) == -1) {
                    perror("recvfrom");
                    exit(1);
            }

            if ((recvfrom(sockfd_A, &linkid,sizeof(float) , 0, (struct sockaddr *)&their_udp_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_A, &bandwidth,sizeof(float) , 0, (struct sockaddr *)&their_udp_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_A, &length,sizeof(float) , 0, (struct sockaddr *)&their_udp_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_A, &velocity,sizeof(float) , 0, (struct sockaddr *)&their_udp_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_A, &noisepower,sizeof(float) , 0, (struct sockaddr *)&their_udp_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }

            if ((recvfrom(sockfd_B, &flagB,sizeof(float) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_B, &linkidB,sizeof(float) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_B, &bandwidthB,sizeof(float) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_B, &lengthB,sizeof(float) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_B, &velocityB,sizeof(float) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            if ((recvfrom(sockfd_B, &noisepowerB,sizeof(float) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                perror("recvfrom");
                exit(1);
            }

            if(flag == 0 && flagB == 0) {
                printf("The AWS received 0 matches from Backend-Server A using UDP over port <24105>\n");
                printf("The AWS received 0 matches from Backend-Server B using UDP over port <24105>\n");
                printf("The AWS sent No Match to the monitor and the client using TCP over ports <26105> and <25105>, respectively\n");
                nomatch = 1;
            } else if(flag == 1){
                printf("The AWS received <1> matches from Backend-Server A using UDP over port <24105>\n");
                printf("The AWS received <0> matches from Backend-Server B using UDP over port <24105>\n");

                addr_lenC = sizeof their_udp_addrC;// This is used for temporary saving Udp source destination
                //send x to serverA,B,C, and recev the caculated value from back-server
                if ((sendto(sockfd_C, &linkid, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &size, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &power, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &bandwidth, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &length, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &velocity, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &noisepower, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }

                printf("The AWS sent link ID=<%d>, size=<%d>, power=<%d>, and link information to Backend-Server C using UDP over port <24105>\n",linkid, size, power);
            } else {
                printf("The AWS received <0> matches from Backend-Server A using UDP over port <24105>\n");
                printf("The AWS received <1> matches from Backend-Server B using UDP over port <24105>\n");

                addr_lenC = sizeof their_udp_addrC;// This is used for temporary saving Udp source destination
                //send x to serverA,B,C, and recev the caculated value from back-server
                if ((sendto(sockfd_C, &linkidB, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &size, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &power, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &bandwidthB, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &lengthB, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &velocityB, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                if ((sendto(sockfd_C, &noisepowerB, sizeof(float), 0, p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: send to C");
                    exit(1);
                }
                printf("The AWS sent link ID=<%d>, size=<%d>, power=<%d>, and link information to Backend-Server C using UDP over port <24105>\n",linkidB, size, power);
            }
            double transdelay;
            double propdelay;
            double endtoend = 0.0;
            if(nomatch == 0) {

                if ((recvfrom(sockfd_C, &transdelay,sizeof(double) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                    perror("recvfrom");
                    exit(1);
                }
                if ((recvfrom(sockfd_C, &propdelay,sizeof(double) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                    perror("recvfrom");
                    exit(1);
                }
                if ((recvfrom(sockfd_C, &endtoend,sizeof(double) , 0, (struct sockaddr *)&their_udp_addrB, &addr_lenB)) == -1) {
                    perror("recvfrom");
                    exit(1);
                }
                printf("The AWS received outputs from Backend-Server C using UDP over port <24105>\n");

            }


            if(send(new_fd_tcp, &nomatch, sizeof(int),0) == -1){
                perror("send");
                exit(1);
            }
            if (nomatch == 0){
                if(send(new_fd_tcp, &endtoend, sizeof(double),0) == -1){
                    perror("send");
                    exit(1);
                }
                printf("The AWS sent delay=<%.2f>ms to the client using TCP over port <25105>\n", endtoend);
            }


            if(send(new_fd_tcpM, &nomatch, sizeof(int),0) == -1){
                perror("send");
                exit(1);
            }
            if (nomatch == 0){
                if(send(new_fd_tcpM, &transdelay, sizeof(double),0) == -1){
                    perror("send");
                    exit(1);
                }
                if(send(new_fd_tcpM, &propdelay, sizeof(double),0) == -1){
                    perror("send");
                    exit(1);
                }
                if(send(new_fd_tcpM, &endtoend, sizeof(double),0) == -1){
                    perror("send");
                    exit(1);
                }
                printf("The AWS sent detailed results to the monitor using TCP over port <26105>\n");
            }
            close(new_fd_tcp);
            exit(0);
        }
        close(new_fd_tcp);
    }
    return 0;
}
