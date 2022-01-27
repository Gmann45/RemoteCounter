#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <sstream>
#include "Server.h"

#define SERVER__BIND_TIMEOUT_S			60

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
	int bindTime = 0;

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
	log->info("Trying to bind socket...");
	while (1) { // Continue to try and bind to socket
		if (bind(serverSock, (struct sockaddr*) &serverSockAddr, sizeof(serverSockAddr)) == -1) {
			if (bindTime++ > SERVER__BIND_TIMEOUT_S) {
				log->warn("Failed to bind socket");
				goto serverFail;
			}
		}
		else {
			log->warn("Successfully bound socket");
			break;
		}

		sleep(1);
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

	log->info("Counter value : %d", counter->getCount());

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

void Server::addConnection(int sock)
{
	connections.push_back(sock);
}

bool Server::removeConnection(int sock)
{
	std::vector<int>::iterator it;

	it = find(connections.begin(), connections.end(), sock);
	if (it == connections.end()) {
		return false;
	}
	connections.erase(it);

	return true;
}

void Server::updateCountOnSocket(int sock)
{
	ssize_t nBytes;
	std::string msg(std::to_string(counter->getCount()));
	msg += "\r\n";

	/* Send message to socket */
	nBytes = send(sock, msg.c_str(), msg.length() + 1, 0);
	if (nBytes <= 0) {
		log->warn("Failed to send data on sock %d", sock);
	}
}

void Server::updateCountOnConnections(void)
{
	ssize_t nBytes;
	std::string msg(std::to_string(counter->getCount()));
	msg += "\r\n";

	/* Loop through sockets and send message */
	for (int sock : connections) {
		nBytes = send(sock, msg.c_str(), msg.length() + 1, 0);
		if (nBytes <= 0) {
			log->warn("Failed to send data on sock %d", sock);
		}
	}
}

bool Server::handleCommand(int sock, char *buf)
{
	std::string cmd(buf);
	std::stringstream ss(cmd);
	std::string tmpString;
	int tmpInt;

	log->info("Command : %s", cmd.c_str());

	if (cmd.find(SERVER__CMD__INCR) != std::string::npos) { // Found command
		if (cmd.find("\r\n", cmd.length() - 2) != std::string::npos) { // Validation check
			while(!ss.eof()) {
				ss >> tmpString;
				if (std::stringstream(tmpString) >> tmpInt) { // Contains valid integer
					counter->incCount(tmpInt);

					updateCountOnConnections();

					break;
				}
			}
		}
	}
	else if (cmd.find(SERVER__CMD__DECR) != std::string::npos) {
		if (cmd.find("\r\n", cmd.length() - 2) != std::string::npos) {
			while(!ss.eof()) {
				ss >> tmpString;
				if (std::stringstream(tmpString) >> tmpInt) {
					counter->decCount(tmpInt);

					updateCountOnConnections();

					break;
				}
			}
		}
	}
	else if (cmd.find(SERVER__CMD__OUTPUT) != std::string::npos) {
		if (cmd.find("\r\n", cmd.length() - 2) != std::string::npos) {
			updateCountOnSocket(sock); // Send message to socket
		}
	}
	else {
		log->info("Unknown command \"%s\" sent to server", buf);
	}

	return true;
}

void Server::cleanUp(void)
{
	/* Close server socket */
	if (close(serverSock) == -1) {
		log->warn("Failed to close server socket");
	}

	/* Close all open sockets */
	for (int sock : connections) {
		if (close(sock) == -1) {
			log->warn("Failed to close socket %d", sock);
		}
	}

	log->info("Cleaned up server");
}
