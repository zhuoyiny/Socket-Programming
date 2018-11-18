//
//  serverC.c
//
//
//  Created by Zhuoying Yi on 11/13/18.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <limits.h>
#include <math.h>

#define BUFFER_UDP_LEN 100000
#define PORT_UDP 23105
#define SERVER_NAME "C"



typedef struct lines{
    int A;
    float B;
    float C;
    float D;
    float E;
    int flag;
} lines;

#define BUFFSIZE 2048

int main(void)
{
    
    struct sockaddr_in c_server_UDP, c_client_UDP;
    socklen_t sockfd_C_UDP;
    socklen_t client_len_UDP = sizeof(c_client_UDP), recv_UDP_len;
    int buff_for_UDP[BUFFER_UDP_LEN];
    
    int linkid;
    int size;
    int power;
    float bandwidth;
    float length;
    float velocity;
    float noisepower;

    double transdelay;
    double propdelay;
    double endtoend;


    if ((sockfd_C_UDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("C UDP created socket failed");
        exit(1);
    }
    memset((char *) &c_server_UDP, 0, sizeof(c_server_UDP));
    c_server_UDP.sin_family = AF_INET;
    c_server_UDP.sin_port = htons(PORT_UDP);
    c_server_UDP.sin_addr.s_addr = htonl(INADDR_ANY);
    if( bind(sockfd_C_UDP , (struct sockaddr*)&c_server_UDP, sizeof(c_server_UDP) ) == -1)
    {
        perror("C UDP bind failed");
        exit(1);
    }
    printf("The Server %s is up and running using UDP on port <%d>\n", SERVER_NAME, PORT_UDP);


    while(1){
        fflush(stdout);

        if (recvfrom(sockfd_C_UDP, &linkid, BUFFER_UDP_LEN, 0, (struct sockaddr *) &c_client_UDP, &client_len_UDP) == -1){
            perror("C UDP receive failed");
        }
        recv_UDP_len = recvfrom(sockfd_C_UDP, &size, BUFFER_UDP_LEN, 0, (struct sockaddr *) &c_client_UDP, &client_len_UDP);
        if (recv_UDP_len == -1) {
            perror("C UDP receive failed");
        }
        recv_UDP_len = recvfrom(sockfd_C_UDP, &power, BUFFER_UDP_LEN, 0, (struct sockaddr *) &c_client_UDP, &client_len_UDP);
        if (recv_UDP_len == -1) {
            perror("C UDP receive failed");
        }
        recv_UDP_len = recvfrom(sockfd_C_UDP, &bandwidth, BUFFER_UDP_LEN, 0, (struct sockaddr *) &c_client_UDP, &client_len_UDP);
        if (recv_UDP_len == -1) {
            perror("C UDP receive failed");
        }
        recv_UDP_len = recvfrom(sockfd_C_UDP, &length, BUFFER_UDP_LEN, 0, (struct sockaddr *) &c_client_UDP, &client_len_UDP);
        if (recv_UDP_len == -1) {
            perror("C UDP receive failed");
        }
        recv_UDP_len = recvfrom(sockfd_C_UDP, &velocity, BUFFER_UDP_LEN, 0, (struct sockaddr *) &c_client_UDP, &client_len_UDP);
        if (recv_UDP_len == -1) {
            perror("C UDP receive failed");
        }
        recv_UDP_len = recvfrom(sockfd_C_UDP, &noisepower, BUFFER_UDP_LEN, 0, (struct sockaddr *) &c_client_UDP, &client_len_UDP);
        if (recv_UDP_len == -1) {
            perror("C UDP receive failed");
        }
        printf("The Server %s received link information of link <%d>, file size <%d>, and signal power <%d>\n", SERVER_NAME, linkid, size, power);

//        printf( " linkid: %d\n", linkid);
//        printf( " size: %d\n", size);
//        printf( " power: %d\n", power);
//        printf( " bandwidth: %f\n", bandwidth);
//        printf( " length: %f\n", length);
//        printf( " velocity: %f\n", velocity);
//        printf( " noisepower: %f\n", noisepower);

        transdelay = 1000 * size / (1000000 * bandwidth * log(1 + pow(10, (power - noisepower)/10))/log(2));

        propdelay = 1000 * 1000 * length / (10000000 * velocity);

        endtoend = transdelay + propdelay;
//        printf( " transdelay: %.2f\n", transdelay);
//        printf( " propdelay: %.2f\n", propdelay);
//        printf( " endtoend: %.2f\n", endtoend);

        printf("The server C finished the calculation for link <%d>\n", linkid);
        if ((sendto(sockfd_C_UDP, &transdelay, sizeof(double), 0,
                    (struct sockaddr *)&c_client_UDP, client_len_UDP)) == -1){
            perror("serverC send back failed");
        }

        if ((sendto(sockfd_C_UDP, &propdelay, sizeof(double), 0,
                    (struct sockaddr *)&c_client_UDP, client_len_UDP)) == -1){
            perror("serverC send back failed");
        }
        if ((sendto(sockfd_C_UDP, &endtoend, sizeof(double), 0,
                    (struct sockaddr *)&c_client_UDP, client_len_UDP)) == -1){
            perror("serverC send back failed");
        }
        printf("The Server C finished sending the output to AWS\n");


    }

    return 0;
}
