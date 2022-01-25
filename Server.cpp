#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Server.h"

Server::Server()
{

}

Server::~Server()
{
	delete counter;
	cleanUp();
}

bool Server::init(int port)
{
	struct sockaddr_in serverSockAddr;

	/* Setup application logging */
	log = Log::getInstance();

	/* Setup server socket */
	serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == -1) {
		log->warn("Failed to create socket");
		return false;
	}

	serverSockAddr.sin_family = AF_INET;
	serverSockAddr.sin_port = htons(port);
	serverSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSock, (struct sockaddr*) &serverSockAddr, sizeof(serverSockAddr)) == -1) {
		log->warn("Failed to bind socket");
		goto serverFail;
	}

	if (listen(serverSock, MAX_CONNECTIONS) == -1) {
		log->warn("Failed to listen to socket connections");
		goto serverFail;
	}

	/* Initialize counter */
	counter = new Counter(0);
	if (counter == nullptr) {
		log->warn("Failed to allocate memory for counter");
		goto serverFail;
	}

	log->info("Successfully initialized server");

	return true;

serverFail:
	if (close(serverSock) == -1) {
		log->warn("Failed to close server socket");
	}

	return false;
}

int Server::getServerSock(void)
{
	return serverSock;
}

int Server::getMaxConnections(void)
{
	return MAX_CONNECTIONS;
}

bool Server::handleCommand(char *buf, int len)
{
	return true;
}

void Server::cleanUp(void)
{
	/* Close server socket */
	if (close(serverSock) == -1) {
		log->warn("Failed to close server socket");
	}
}
