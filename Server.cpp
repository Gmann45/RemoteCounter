#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <sstream>
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

void Server::updateCountOnConnections(void)
{
	ssize_t nBytes;
	std::string msg(std::to_string(counter->getCount()));

	log->info("Updating sockets with value : %s", msg.c_str());

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

	if (cmd.find(SERVER__CMD__INCR) != std::string::npos) {
		log->info("Handling INCR command");
		if (cmd.find("\r\n", cmd.length() - 2) != std::string::npos) { // Validation check
			log->info("Contains escape chars");
			while(!ss.eof()) {
				ss >> tmpString;
				if (std::stringstream(tmpString) >> tmpInt) { // Contains valid integer
					log->info("Found int %d in command", tmpInt);
					counter->incCount(tmpInt);

					updateCountOnConnections();
				}
			}
		}
	}
	else if (cmd.find(SERVER__CMD__DECR) != std::string::npos) {
		if (cmd.find("\r\n", cmd.length() - 2) != std::string::npos) {

		}
	}
	else if (cmd.find(SERVER__CMD__OUTPUT) != std::string::npos) {
		if (cmd.find("\r\n", cmd.length() - 2) != std::string::npos) {

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
}
