#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
int initSocket(int* pValid, struct sockaddr_in* pAdresse, int port, int doBind);
void sendSYN(int desc,struct sockaddr* pAdrr, int size, int type);
void connexionServ(int desc, struct sockaddr* pAddr, socklen_t *pSizeAddr, int portUtil);
int connexionClient(int desc, struct sockaddr* pAddr, socklen_t *pSizeAddr);
void sendFile(char *fileName, int descUtil,struct sockaddr* pUtil, socklen_t sizeUtilAddr);
