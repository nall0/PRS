#include "utilPRS.h"

int main() {
	struct sockaddr_in clientHS;
	int publicPort = 2000;
	int privatePort = publicPort;
	int enableOption = 1;
	socklen_t sizeClientHS = sizeof(clientHS);
	fd_set set;
	int nbMaxClient = 100;

	int descHS = initSocket(&enableOption, &clientHS, publicPort);
	
	while(1) {
		FD_ZERO(&set);
		
		select(3+nbMaxClient, &set, NULL, NULL, NULL);
		
		if(FD_ISSET(descHS, &set) == TRUE) {
			//a new client arrive
			handshake(descHS, (struct sockaddr *) &clientHS, &sizeClientHS, ++privatePort);
			printf("it is a success !\n");
		}
		
	}


	return 0;
}
