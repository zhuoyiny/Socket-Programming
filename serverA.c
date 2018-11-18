#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <limits.h>

#define BUFFER_UDP_LEN 100000
#define PORT_UDP 21105
#define SERVER_NAME "A"

struct sockaddr_in a_server_UDP, a_client_UDP;
int sockfd_A_UDP, client_len_UDP = sizeof(a_client_UDP) , recv_UDP_len;
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

#define BUFFSIZE 2048

void read_file(int linkid) {

  FILE *file = fopen("./database_a.csv", "r");
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

    printf( "The server %s has found <%d> match\n", SERVER_NAME, struc[i].flag);
     if ((sendto(sockfd_A_UDP, &struc[i].flag, sizeof(int), 0,
          (struct sockaddr *)&a_client_UDP, client_len_UDP)) == -1){
         perror("serverA send back failed");
     }

    if ((sendto(sockfd_A_UDP, &struc[i].A, sizeof(float), 0,
                (struct sockaddr *)&a_client_UDP, client_len_UDP)) == -1){
        perror("serverA send back failed");
    }
    if ((sendto(sockfd_A_UDP, &struc[i].B, sizeof(float), 0,
                (struct sockaddr *)&a_client_UDP, client_len_UDP)) == -1){
        perror("serverA send back failed");
    }
    if ((sendto(sockfd_A_UDP, &struc[i].C, sizeof(float), 0,
                (struct sockaddr *)&a_client_UDP, client_len_UDP)) == -1){
        perror("serverA send back failed");
    }
    if ((sendto(sockfd_A_UDP, &struc[i].D, sizeof(float), 0,
                (struct sockaddr *)&a_client_UDP, client_len_UDP)) == -1){
        perror("serverA send back failed");
    }
    if ((sendto(sockfd_A_UDP, &struc[i].E, sizeof(float), 0,
                 (struct sockaddr *)&a_client_UDP, client_len_UDP)) == -1){
        perror("serverA send back failed");
    }
     printf("The Server %s finished sending the output to AWS\n", SERVER_NAME);


  fclose(file);

}


int main(void)
{
    if ((sockfd_A_UDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("UDP created socket failed");
        exit(1);
    }
    memset((char *) &a_server_UDP, 0, sizeof(a_server_UDP));
    a_server_UDP.sin_family = AF_INET;
    a_server_UDP.sin_port = htons(PORT_UDP);
    a_server_UDP.sin_addr.s_addr = htonl(INADDR_ANY);
    if( bind(sockfd_A_UDP , (struct sockaddr*)&a_server_UDP, sizeof(a_server_UDP) ) == -1)
    {
        perror("UDP bind failed");
        exit(1);
    }
    printf("The Server %s is up and running using UDP on port <%d>\n", SERVER_NAME, PORT_UDP);
    while(1){
        fflush(stdout);
        recv_UDP_len = recvfrom(sockfd_A_UDP, &linkid, BUFFER_UDP_LEN, 0,
                                (struct sockaddr *) &a_client_UDP, &client_len_UDP);
        if (recv_UDP_len == -1)
        {
            perror("UDP receive failed");
        }
        printf("The Server %s has received Link id <%d>\n", SERVER_NAME, linkid);

        read_file(linkid);

    }

    return 0;
}
