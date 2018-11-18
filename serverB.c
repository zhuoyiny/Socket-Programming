//
//  serverB.c
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

#define BUFFER_UDP_LEN 100000
#define PORT_UDP 22105 
#define SERVER_NAME "B"

struct sockaddr_in b_server_UDP, b_client_UDP;
int sockfd_B_UDP, client_len_UDP = sizeof(b_client_UDP) , recv_UDP_len;
int buff_for_UDP[BUFFER_UDP_LEN];
int linkid;

typedef struct lines{
    int A;
    float B;
    float C;
    float D;
    float E;
    int flag;
} lines;

#define BUFFSIZE 1024

void read_file(int linkid) {

    FILE *file = fopen("./database_b.csv", "r");
    if (file == NULL){
        perror("file open failed");
    }
    char line[BUFFSIZE];
    char *Token;
    char *TokenB;
    char *TokenC;
    char *TokenD;
    char *TokenE;
    struct lines struc[BUFFSIZE];

    int index = 0;
    int i = 0;
    while(fgets(line, BUFFSIZE, file) != NULL){

        Token = strtok(line, ",");
        //      printf( " linkid: %d\n", linkid);
        if (linkid == atoi(Token)) {
            struc[i].flag = 1;
            struc[i].A = atoi(Token);
            TokenB = strtok(NULL, ",");
            struc[i].B = atof(TokenB);
            TokenC = strtok(NULL, ",");
            struc[i].C = atof(TokenC);
            TokenD = strtok(NULL, ",");
            struc[i].D = atof(TokenD);
            TokenE = strtok(NULL, ",");
            struc[i].E = atof(TokenE);
            index++;
//            printf( " tokenA: %s\n", Token);
//            printf( " tokenB: %s\n", TokenB);
//            printf( " tokenC: %s\n", TokenC);
//            printf( " tokenD: %s\n", TokenD);
//            printf( " tokenE: %s\n", TokenE);
            break;
        } else {
            struc[i].flag = 0;
            struc[i].A = 0;
            struc[i].B = 0;
            struc[i].C = 0.0;
            struc[i].D = 0.0;
            struc[i].E = 0.0;
        }
        i++;
    }
//    printf("count: %d\n", index);
//    printf("i: %d\n", i);
//    printf( " struc_flag: %d\n", struc[i].flag);
//    printf( " struc_A: %d\n", struc[i].A);
//    printf( " struc_B: %d\n", struc[i].B);
//    printf( " struc_C: %f\n", struc[i].C);
//    printf( " struc_D: %f\n", struc[i].D);
//    printf( " struc_E: %f\n", struc[i].E);

    printf( "The server %s has found <%d> match\n", SERVER_NAME, struc[i].flag);
    if ((sendto(sockfd_B_UDP, &struc[i].flag, sizeof(int), 0,
                (struct sockaddr *)&b_client_UDP, client_len_UDP)) == -1){
        perror("serverB send back failed");
    }

    if ((sendto(sockfd_B_UDP, &struc[i].A, sizeof(float), 0,
                (struct sockaddr *)&b_client_UDP, client_len_UDP)) == -1){
        perror("serverB send back failed");
    }
    if ((sendto(sockfd_B_UDP, &struc[i].B, sizeof(float), 0,
                (struct sockaddr *)&b_client_UDP, client_len_UDP)) == -1){
        perror("serverB send back failed");
    }
    if ((sendto(sockfd_B_UDP, &struc[i].C, sizeof(float), 0,
                (struct sockaddr *)&b_client_UDP, client_len_UDP)) == -1){
        perror("serverB send back failed");
    }
    if ((sendto(sockfd_B_UDP, &struc[i].D, sizeof(float), 0,
                (struct sockaddr *)&b_client_UDP, client_len_UDP)) == -1){
        perror("serverB send back failed");
    }if ((sendto(sockfd_B_UDP, &struc[i].E, sizeof(float), 0,
                 (struct sockaddr *)&b_client_UDP, client_len_UDP)) == -1){
        perror("serverB send back failed");
    }
    printf("The Server %s finished sending the output to AWS\n", SERVER_NAME);



    // free(line);
    fclose(file);

}


int main(void)
{
    if ((sockfd_B_UDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("UDP created socket failed");
        exit(1);
    }
    memset((char *) &b_server_UDP, 0, sizeof(b_server_UDP));

    b_server_UDP.sin_family = AF_INET;
    b_server_UDP.sin_port = htons(PORT_UDP);
    b_server_UDP.sin_addr.s_addr = htonl(INADDR_ANY);
    if( bind(sockfd_B_UDP , (struct sockaddr*)&b_server_UDP, sizeof(b_server_UDP) ) == -1)
    {
        perror("UDP bind failed");
        exit(1);
    }
    printf("The Server %s is up and running using UDP on port <%d>\n", SERVER_NAME, PORT_UDP);
    while(1){
        fflush(stdout);

        recv_UDP_len = recvfrom(sockfd_B_UDP, &linkid, BUFFER_UDP_LEN, 0,
                                (struct sockaddr *) &b_client_UDP, &client_len_UDP);
        if (recv_UDP_len == -1)
        {
            perror("UDP receive failed");
        }
        printf("The Server %s received input <%d>\n", SERVER_NAME, linkid);

        read_file(linkid);

    }

    return 0;
}
