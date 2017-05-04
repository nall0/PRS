#include "utilPRS.h"

void handleError(int var, char *functionName) {
	if (var < 0) {
		perror("bind");
		printf("error in function %s\nExit\n",functionName);
		exit(-1);
	}
}

int initSocket(int* pValid, struct sockaddr_in* pAdresse, int port) {
	int desc; //file descripteur for the socket
	desc = socket(AF_INET, SOCK_DGRAM,0); //SOCK_DGRAM indicate that we use UDP
	handleError(desc,"socket");

	//Handling some options on socket
	int sockOption = setsockopt(desc, SOL_SOCKET, SO_REUSEADDR, pValid, sizeof(int));
	handleError(sockOption, "setsockopt");

	//Associating struc adresse with the port
	pAdresse->sin_family= AF_INET;
	pAdresse->sin_port= htons(port);
	pAdresse->sin_addr.s_addr= htonl(INADDR_ANY);

	//associating  the socket with the adresse
	int bindRes = bind(desc, (struct sockaddr*) pAdresse, sizeof(*pAdresse));
	handleError(bindRes, "bind");

	return desc;
}

void handshake(int desc, struct sockaddr* pAddr, socklen_t *pSizeAddr, int privatePort) {
	char SYN_ACK[12];

	//reception de SYN
	memset(SYN_ACK,0,strlen(SYN_ACK));
	recvfrom(desc,SYN_ACK,sizeof(SYN_ACK),0, pAddr,pSizeAddr);
	if (strcmp(SYN_ACK , "SYN") != 0 ) {
		printf("il faut envyer SYN au début d'une connexion\n recu : %s\n", SYN_ACK);
		exit(1);
	}

	//envoi de SYN_ACK0000
	memset(SYN_ACK,0,strlen(SYN_ACK));
	sprintf(SYN_ACK, "SYN-ACK");
	char charPrivatePort[6];
	sprintf(charPrivatePort, "%d", privatePort); //convertion du port privé en string
	strcat(SYN_ACK, charPrivatePort);
	sendto(desc,SYN_ACK,sizeof(SYN_ACK),0, pAddr, *pSizeAddr);

	//reception de ACK
	memset(SYN_ACK,0,strlen(SYN_ACK));
	recvfrom(desc,SYN_ACK,sizeof(SYN_ACK),0, pAddr,pSizeAddr);
	if (strcmp(SYN_ACK , "ACK") != 0 ) {
		printf("il faut envyer ACK au début d'une connexion\n");
		exit(1);
	}
}

int ackToInt(char ackReceive[]) {
	//fonction qui rend le numéro acquitté contenu dans le message recu
	char num[7];
	sprintf(num,"%s",ackReceive+3);
	return atoi(num);
}

int sendSeq(int cwnd, int seqNum, char *fileName, int descUtil,struct sockaddr* pUtil, socklen_t sizeUtilAddr) {
	int res = 1;
	char read[SEGSIZE-6]; //contenu lu dans le fichier
	char msg[SEGSIZE]; //message envoyé, avec le numéro de sequence au debut
	int sndto, readSize;
	int sendingNumber = 0;
	int decalage = seqNum*SAMPLESIZE;

	FILE *f1;
	f1 = fopen(fileName,"rb");
	fseek(f1, decalage, SEEK_SET);
	

	while(sendingNumber<cwnd && feof(f1) == FALSE) {
		sprintf(msg, "%6d", ++seqNum);
		readSize = fread(msg+6, sizeof(char), SAMPLESIZE, f1);
		printf("envoi du segment %d\n",seqNum);
		sndto = sendto(descUtil,msg,readSize+6,0,pUtil, sizeUtilAddr);
		handleError(sndto, "sendto");
		memset(read,0,SEGSIZE-6);
		memset(msg,0,SEGSIZE);
		sendingNumber++;
	}

	if (feof(f1) == TRUE) {
		printf("envoie fin\n");
		sendto(descUtil,"FIN",sizeof("FIN"),0,pUtil,sizeUtilAddr);
		res = 0;
	}

	fclose(f1);
	return res;
}


double timeval_substract(struct timeval *x, struct timeval *y)  
{  
    double diff = x->tv_sec - y->tv_sec;  
    diff += (x->tv_usec - y->tv_usec)/1000000.0;  

    return diff;  
}

int calcRTT( struct timeval t1, struct timeval t2, struct timeval *pRTT) {
	
	double newRTT = timeval_substract(&t2,&t1);
	double oldRTT = timevalToDouble(*pRTT);
	newRTT = (1-ALPHA)*oldRTT + ALPHA * newRTT;
	doubleToTimeval(newRTT,pRTT);
	return 0;
}

int doubleToTimeval (double time, struct timeval *pTime) {
	pTime->tv_sec = (int) floor(time);
	pTime->tv_usec = (int) 1000000*(time - floor(time));
	return 0;
}

double timevalToDouble (struct timeval time) {
	double t = time.tv_sec + time.tv_usec/1000000.0;
    return t;
}
