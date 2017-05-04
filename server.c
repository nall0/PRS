#include "utilPRS.h"

int main(int argc, char * argv[]) {
	struct sockaddr_in clientHS, client;
	int publicPort = atoi(argv[1]);
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
	struct timeval rtt;
	rtt.tv_sec = 0;
	rtt.tv_usec = 200000;
	struct timeval timeout;
	struct timeval timeoutArrival;
	timeoutArrival.tv_sec = 1;
	struct timeval t1,t2;

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
		}

		//sending
		if(cont) {
			//socket ready to send things
			printf("cwnd = %d\n", cwnd);
			cont = sendSeq(cwnd, seqNum, fileName, desc,(struct sockaddr *) &client, sizeClient);
			seqNum = seqNum + cwnd;
		}

		//reception des ACK
		i = 0;
		cont2 = 1;
		while(i<cwnd && cont2) {
			i++;
			
			if (i==1)
				gettimeofday(&t1,NULL);
			timeout = rtt;
			ret = select(3+nbMaxClient, &setReceive, NULL, NULL, &timeout);
			printf("select ACK : %d \n",ret);
			handleError(ret, "select");
			if(FD_ISSET(desc, &setReceive)) {
				//socket ready to receive things
				printf("receiving ACK\n");
				recvfrom(desc,ackReceive, 10, 0, (struct sockaddr *) &client, &sizeClient);
				if (i==1) {
					gettimeofday(&t2,NULL);
					calcRTT(t1,t2,&rtt);
					
					printf("rtt = %d s%d us\n", (int) rtt.tv_sec, (int)rtt.tv_usec );
				}

				printf("%s\n", ackReceive);
				if (acquitte < ackToInt(ackReceive)) {
					acquitte = ackToInt(ackReceive);
				}

			} else {
				//ACK
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
