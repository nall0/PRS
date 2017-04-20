#include "utilPRS.h"

int main() {
	struct sockaddr_in clientHS, client;
	int publicPort = 2000;
	int privatePort = publicPort;
	int enableOption = 1;
	socklen_t sizeClientHS = sizeof(clientHS);
	socklen_t sizeClient = sizeof(client);
	fd_set set, setACK;
	int nbMaxClient = 100;
	int ret; //retour du select
	int descHS = initSocket(&enableOption, &clientHS, publicPort);
	int desc;
	char fileName[SEGSIZE];
	int seqNum = 0;
	char ackReceive[10];
	int cwnd = 2; //fenetre d'envoi pour Slow Start
	//int sstresh = 50; //seuil avant de passer en congestion avoidance
	//int acquitte = 0;

	while(1) {
		FD_ZERO(&set);
		FD_ZERO(&setACK);

		FD_SET(descHS,&set);

		ret = select(3+nbMaxClient, &setACK, &set, NULL, NULL);
		handleError(ret, "select");
		printf("1\n");

		if(FD_ISSET(descHS, &set) == TRUE) {
			printf("nouveau client\n");
			//a new client arrive
			handshake(descHS, (struct sockaddr *)&clientHS, &sizeClientHS, ++privatePort);
			printf("Handshake succeeded\nprivate port sent = %d\n",privatePort);

			//new socket declaration
			socklen_t sizeClient = sizeof(client);
			desc = initSocket(&enableOption, &client, privatePort);
			FD_SET(desc, &set);
			FD_SET(desc,&setACK);

			//reception du nom du fichier
			recvfrom(desc,fileName,sizeof(fileName),0, (struct sockaddr *)&client,&sizeClient);
			printf("Required file : |%s|\n", fileName);

		}
		printf("2\n");
		if(FD_ISSET(desc, &set) == TRUE) {
			//socket ready to send things
			sendSeq(cwnd, seqNum, fileName, desc,(struct sockaddr *) &client, sizeClient);
			seqNum = seqNum + cwnd;

		}

		select(3+nbMaxClient, &setACK, &set, NULL, NULL);		
		if(FD_ISSET(desc, &setACK) == TRUE) {
			//socket ready to receive things
			printf("waiting for ack\n");
			recvfrom(desc,ackReceive, 10, 0, (struct sockaddr *) &client, &sizeClient);
			printf("%s\n", ackReceive);
			/*if (acquitte < ackToInt(ackReceive)) {
				acquitte = ackToInt(ackReceive);
			}*/

		}

		/*if (acquitte != seqNum) {
			//A FAIRE : renvoi à partir du segment numéro acquitte
			cwnd = cwnd/2;
		} else if (cwnd<sstresh){
			cwnd = cwnd*2;
		} else {
			cwnd++;
		}*/

	}


	return 0;
}
