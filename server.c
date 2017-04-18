#include "utilPRS.h"

int main() {
	printf("salut\n");
	printf("salut salut\n");
	struct sockaddr_in clientHS;
	int publicPort = 2000;
	int privatePort = publicPort;
	int enableOption = 1;
	socklen_t sizeClientHS = sizeof(clientHS);
	fd_set set;
	int nbMaxClient = 100;
	int descHS = initSocket(&enableOption, &clientHS, publicPort);
	char fileName[SEGSIZE];

	while(1) {
		FD_ZERO(&set);
		FD_SET(descHS, &set);

		int ret = select(3+nbMaxClient, &set, NULL, NULL, NULL);

		if(ret == -1) {
		printf("select failed\n");
		}

		if(FD_ISSET(descHS, &set) == TRUE) {
			//a new client arrive
			handshake(descHS, (struct sockaddr *)&clientHS, &sizeClientHS, ++privatePort);
			printf("Handshake succeeded\nprivate port sent = %d\n",privatePort);

			//new socket declaration
			struct sockaddr_in client;
			socklen_t sizeClient = sizeof(client);
			int desc = initSocket(&enableOption, &client, privatePort);

			printf("yo\n");
			//reception du nom et envoi du fichier
			recvfrom(desc,fileName,sizeof(fileName),0, (struct sockaddr *)&client,&sizeClient);
			printf("Required file : |%s|\n", fileName);
			sendFile(fileName, desc,(struct sockaddr *)&client, sizeClient);
		}

	}


	return 0;
}
