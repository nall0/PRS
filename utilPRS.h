#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <math.h>

#define SAMPLESIZE 1000
#define SEGSIZE SAMPLESIZE + 6
#define NUMSEG 6
#define ALPHA 0.2

#define TRUE  1
#define FALSE 0

void handleError(int var, char *functionName);
int initSocket(int* pValid, struct sockaddr_in* pAdresse, int port);
void handshake(int desc, struct sockaddr* pAddr, socklen_t *pSizeAddr, int privatePort);
int ackToInt(char ackReceive[]);
int sendSeq(int cwnd, int seqNum, char *fileName, int descUtil,struct sockaddr* pUtil, socklen_t sizeUtilAddr);
double timeval_substract(struct timeval *x, struct timeval *y);
int calcRTT( struct timeval t1, struct timeval t2, struct timeval *pRTT);
int doubleToTimeval (double time, struct timeval *pTime);
double timevalToDouble (struct timeval time);
