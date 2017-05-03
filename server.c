#include "utilPRS.h"

int main() {
	struct sockaddr_in clientHS, client;
	int publicPort = 2000;
	int privatePort = publicPort;
	int enableOption = 1;
	socklen_t sizeClientHS = sizeof(clientHS);
	socklen_t sizeClient = sizeof(client);
	fd_set setSend, setReceive;
	int nbMaxClient = 50;
	int ret; //retour du select
	int descHS = initSocket(&enableOption, &clientHS, publicPort);
	int desc;
	char fileName[SEGSIZE];
	int seqNum = 0;
	char ackReceive[10];
	int cwnd = 2; //fenetre d'envoi pour Slow Start
	int sstresh = 50; //seuil avant de passer en congestion avoidance
	int acquitte = 0;
	int cont = 1;
	int cont2 = 1;
	int i=0;
	struct timeval timeout;
	timeout.tv_sec = 1;
	//timeout.tv_usec = 150000;
	struct timeval timeoutArrival;
	timeoutArrival.tv_sec = 1;

	FD_ZERO(&setSend);
	FD_ZERO(&setReceive);

	FD_SET(descHS,&setReceive);

	while(1) {


		ret = select(3+nbMaxClient, &setReceive, NULL, NULL, &timeoutArrival);
		handleError(ret, "select");

		if(FD_ISSET(descHS, &setReceive)) {
			printf("New client :");
			//a new client arrive
			handshake(descHS, (struct sockaddr *)&clientHS, &sizeClientHS, ++privatePort);
			printf("Handshake succeeded. Private port = %d\n",privatePort);

			//new socket declaration
			socklen_t sizeClient = sizeof(client);
			desc = initSocket(&enableOption, &client, privatePort);
			FD_SET(desc, &setSend);
			FD_SET(desc, &setReceive);

			//reception du nom du fichier
			recvfrom(desc,fileName,sizeof(fileName),0, (struct sockaddr *)&client,&sizeClient);
			printf("Required file : |%s|\n", fileName);
			FILE *f1;
			f1 = fopen(fileName,"rb");

		}

		//printf("waiting for send\n");
		if(cont) {
			//socket ready to send things
			printf("cwnd = %d\n", cwnd);
			cont = sendSeq(cwnd, seqNum, fileName, desc,(struct sockaddr *) &client, sizeClient);
			seqNum = seqNum + cwnd;
		}

		//printf("waiting for ack\n");
		//reception des ACK
		i = 0;
		cont2 = 1;
		while(i<cwnd && cont2) {
			i++;
			timeout.tv_sec = 0;
			timeout.tv_usec = 500000;
			ret = select(3+nbMaxClient, &setReceive, NULL, NULL, &timeout);
			printf("select ACK : %d \n",ret);
			handleError(ret, "select");
			if(FD_ISSET(desc, &setReceive)) {
				//socket ready to receive things
				printf("receiving ACK\n");
				recvfrom(desc,ackReceive, 10, 0, (struct sockaddr *) &client, &sizeClient);
				printf("%s\n", ackReceive);
				if (acquitte < ackToInt(ackReceive)) {
					acquitte = ackToInt(ackReceive);
				}

			} else {
				//printf("ACK dropped : cwnd = cwnd/2, et renvoi depuie acquitte\n");
				cont2 = 0;
			}
		}
		//ACK control
		if (acquitte != seqNum) {
			seqNum = acquitte; //renvoi depuis le plus grand segment acquitté
			cwnd = cwnd/2; //division de la fenetre d'envoi par deux
			if (cwnd == 0)
				cwnd = 1;
		} else if (cwnd<sstresh){
			cwnd = cwnd*2;
		} else {
			cwnd++;
		}
		//end of transmission
		if (cont == FALSE) {
			printf("socket closed\n");
			close(desc);
			seqNum = 0;
			cwnd = 2;
		}


			FD_ZERO(&setSend);
			FD_ZERO(&setReceive);

			FD_SET(descHS,&setReceive);
			FD_SET(desc, &setReceive);
		}


	return 0;
}
