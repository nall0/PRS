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
	int descHS = initSocket(&enableOption, &clientHS, publicPort);
	int desc;
	char fileName[SEGSIZE];
	char ackReceive[10];

	while(1) {
		FD_ZERO(&set);
		FD_ZERO(&setACK);

		FD_SET(descHS, &set);
		FD_SET(desc, &set);
		FD_SET(desc,&setACK);

		int ret = select(3+nbMaxClient, &setACK, &set, NULL, NULL);

		handleError(ret, "select");

		if(FD_ISSET(descHS, &set) == TRUE) {
			//a new client arrive
			handshake(descHS, (struct sockaddr *)&clientHS, &sizeClientHS, ++privatePort);
			printf("Handshake succeeded\nprivate port sent = %d\n",privatePort);

			//new socket declaration
			socklen_t sizeClient = sizeof(client);
			desc = initSocket(&enableOption, &client, privatePort);

			printf ("get there\n");
			//reception du nom et envoi du fichier
			recvfrom(desc,fileName,sizeof(fileName),0, (struct sockaddr *)&client,&sizeClient);
			printf("Required file : |%s|\n", fileName);

			FD_SET(desc, &set);
		}

		if(FD_ISSET(desc, &set) == TRUE) {
			//socket ready to send things
			sendFile(fileName, desc,(struct sockaddr *) &client, sizeClient);

		}

		if(FD_ISSET(desc, &setACK) == TRUE) {
			//socket ready to receive things
			recvfrom(desc,ackReceive, 10, 0, (struct sockaddr *) &client, &sizeClient);
			printf("%s\n", ackReceive);

		}

	}


	return 0;
}
