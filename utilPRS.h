#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>

#define RCVSIZE 1024
#define SEGSIZE 1006
#define SAMPLESIZE 1000
#define NUMSEG 6
#define SYN 1
#define ACK 2
#define SYNACK 3

#define TRUE  1
#define FALSE 0

void handleError(int var, char *functionName);
int initSocket(int* pValid, struct sockaddr_in* pAdresse, int port);
void handshake(int desc, struct sockaddr* pAddr, socklen_t *pSizeAddr, int privatePort);
void ajoutDebut(char msg[], char read[], int seqNum);
int ackToInt(char ackReceive[]);
int sendSeq(int cwnd, int seqNum, char *fileName, int decalage, int descUtil,struct sockaddr* pUtil, socklen_t sizeUtilAddr);
