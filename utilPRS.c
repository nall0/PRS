#include "utilPRS.h"

void handleError(int var, char *functionName) {
	if (var < 0) {
		printf("error in function %s\nExit\n",functionName);
		exit(-1);
	}
}

int initSocket(int* pValid, struct sockaddr_in* pAdresse, int port, int doBind) {
	int desc; //file descripteur for the socket
	desc = socket(AF_INET, SOCK_DGRAM,0);	//SOCK_DGRAM indicate that we use UDP
	handleError(desc,"socket");
	
	//Handling some options on socket
	setsockopt(desc, SOL_SOCKET, SO_REUSEADDR, pValid, sizeof(int));
	
	//Associating struc adresse with the port
	pAdresse->sin_family= AF_INET;
	pAdresse->sin_port= htons(port);
	pAdresse->sin_addr.s_addr= htonl(INADDR_ANY);
	
	//associating  the socket with the adresse
	if (doBind == TRUE) {
		int bindRes = bind(desc, (struct sockaddr*) pAdresse, sizeof(*pAdresse));
		handleError(bindRes, "bind");
	}

	return desc;
}

void sendSYNACK(int desc,struct sockaddr* pAddr, int size, int type) {
	char str[RCVSIZE];
	if (type == SYN)
		sprintf(str,"SYN\n");
	else if (type == ACK)
		sprintf(str,"ACK\n");
	else if (type == SYNACK)
		sprintf(str,"SYN+ACK\n");
	else
		printf("error in sending ACK or SYN\n");
	sendto(desc,str,strlen(str),0, pAddr, size);
}

void connexionServ(int desc, struct sockaddr* pAddr, socklen_t *pSizeAddr, int portUtil) {
	char buffer[RCVSIZE];
	memset(buffer,0,RCVSIZE);
	recvfrom(desc,buffer,RCVSIZE,0, pAddr,pSizeAddr); 
	printf("RECEIVED MESSAGE : %s",buffer);
	memset(buffer,0,RCVSIZE);	
		
	if (strcmp( buffer , "SYN") ) {
		sendSYNACK(desc, pAddr, *pSizeAddr ,SYNACK);			
		printf("SYN+ACK send\nwaiting for ACK\n");
		recvfrom(desc,buffer,RCVSIZE,0, pAddr, pSizeAddr); 
		printf("RECEIVED MESSAGE : %s",buffer);
		memset(buffer,0,RCVSIZE);
		if (strcmp(buffer, "ACK")) {
			printf("connexion established\n");
			//AND NOW : LET'S SEND THE UTIL PORT !
			sprintf(buffer,"%d",portUtil);
			printf("sending util port (%d)\n",portUtil);
			sendto(desc,buffer,strlen(buffer),0, pAddr, *pSizeAddr);
		
		}
	}
	
}

int connexionClient(int desc, struct sockaddr* pAddr, socklen_t *pSizeAddr) {	
	int port;
	char buffer[RCVSIZE];
	sendSYNACK(desc, pAddr, *pSizeAddr,SYN);
	printf("SYN send\nwaiting for SYN+ACK\n");
	sleep(1);
	recvfrom(desc,buffer,RCVSIZE,0, pAddr,pSizeAddr); 
	printf("MESSAGE RECEIVED : %s",buffer);
	if (strcmp(buffer,"SYN+ACK")) {
		sendSYNACK(desc, pAddr, *pSizeAddr,ACK);
		printf("ACK send\nconnexion established\n");
	}
	memset(buffer,0,RCVSIZE);
	//LET'S RECEIVE THE UTIL PORT !	
	recvfrom(desc,buffer,RCVSIZE,0, pAddr,pSizeAddr);
	printf("MESSAGE RECEIVED (port util) : %s\n",buffer);
	port = atoi(buffer);
	memset(buffer,0,RCVSIZE);	
	return port;
}
	
void sendFile(char *fileName, int descUtil,struct sockaddr* pUtil, socklen_t sizeUtilAddr) {
	char msg[SEGSIZE];
	char str[SAMPLESIZE]; //string to receive the file's segments
	
	int sndto;

	FILE *f1;
	f1 = fopen(fileName,"rb");
	
	//1st step : get the file length
	fseek(f1, 0, SEEK_END); //on se place à la fin du fichier
	int fileSize = ftell(f1);	//getting the current position, i.e. the file size
	int lastSegmentSize = fileSize % SEGSIZE;
	int numberOfSegments = (int) fileSize/SEGSIZE + 1;
	fseek(f1,0,SEEK_SET);
	printf("file size : %d\nlast segment size : %d\nnumber of  segments : %d\n",fileSize, lastSegmentSize, numberOfSegments);
	
	int i;
	for (i=1 ; i< numberOfSegments ; i++) {
		if (fread(str,1, SAMPLESIZE, f1) != SAMPLESIZE) {
			printf("erreur\n");
		}
		sprintf(msg,"%+6d%s",i,str);
		printf("envoi du segment %d \n",i); 	
		sndto = sendto(descUtil,msg,sizeof(msg),0,pUtil, sizeUtilAddr);
		handleError(sndto, "sendto");
		memset(str,0,SEGSIZE);
		memset(msg,0,SEGSIZE);
	}
	
	if (fread(str, 1, lastSegmentSize, f1) != lastSegmentSize) {
		printf("probleme dans la gestion de la taille du dernier segment \n");
	}
	sprintf(msg,"%+6d%s",i,str);
	printf("envoi du segment %d \n",i); 	
	sndto = sendto(descUtil,msg,sizeof(msg),0,pUtil, sizeUtilAddr);
	memset(str,0,SEGSIZE);
	memset(msg,0,SEGSIZE);
	
	fclose(f1);
}


	


