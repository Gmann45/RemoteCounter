#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <new>
#include "Log.h"
#include "Server.h"

Log* Log::inst_ = nullptr;
char* Log::progname = nullptr;
Server* server;

static void _termSignalHandler(int sig)
{
	/* Server cleanup */
	if (server) {
		server->cleanUp();
	}

	exit(0);
}

static bool _initSignalHandler(void)
{
	struct sigaction sa;

	sa.sa_handler = _termSignalHandler;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	Log *log = Log::getInstance();
	int numFds, epollFd, numEvents, addEvents = 0;
	struct epoll_event event, *events;

	/* Setup signal handler */
	if (!_initSignalHandler()) {
		log->err("Failed to initialize term signal handler");
		exit(1);
	}

	server = new Server();
	if (server == nullptr) {
		log->err("Failed to create server");
		exit(1);
	}

	/* Initialize server */
	if (!server->init()) {
		log->err("Failed to initialize server");
		exit(1);
	}

	/* Setup epoll events */
	epollFd = epoll_create1(0);
	if (epollFd == -1) {
		log->err("Failed to create epoll fd");
		exit(1);
	}

	event.events = EPOLLIN;
	event.data.fd = server->getServerSock();
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, server->getServerSock(), &event) == -1) {
		log->err("Failed to add server socket to epoll fd");
		exit(1);
	}

	numEvents = 1;
	events = new epoll_event[numEvents];
	if (events == nullptr) {
		log->err("Failed to allocate memory for events");
		exit(1);
	}

	while(1) {
		/* Wait on events */
		numFds = epoll_wait(epollFd, events, numEvents, -1);
		if (numFds == -1) {
			log->err("Failed to wait for epoll event");
			exit(1);
		}

		for (int i = 0; i < numFds; i++) {
			if (events[i].data.fd == server->getServerSock()) {
				int connSock;

				if (numEvents + 1 > server->getMaxConnections()) {
					log->warn("Reached connection limit");
					break;
				}

				connSock = accept(server->getServerSock(), NULL, NULL);
				if (connSock == -1) {
					log->warn("Failed to accept incoming socket");
					break;
				}

				if (fcntl(connSock, F_SETFL, O_NONBLOCK) == -1) {
					log->warn("Failed to set socket to non-blocking");
					close(connSock);
					break;
				}

				event.events = EPOLLIN | EPOLLET;
				event.data.fd = connSock;
				if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connSock, &event) == -1) {
					log->warn("Failed to add socket to epoll");
					close(connSock);
					break;
				}

				/* Increase event count */
				struct epoll_event *tmpEvents = new epoll_event[numEvents+1];
				if (tmpEvents == nullptr) {
					log->warn("Failed to allocate memory for events");
					epoll_ctl(epollFd, EPOLL_CTL_DEL, connSock, NULL);
					close(connSock);
					break;
				}
				delete[] events;
				events = tmpEvents;
				numEvents++;
				server->addConnection(connSock);

				log->info("Added new socket : %d", connSock);
			}
			else {
				int connSock = events[i].data.fd;
				char buf[32];
				int nBytes;

				memset(buf, 0, sizeof(buf));

				nBytes = recv(connSock, buf, sizeof(buf), 0);
				if (nBytes > 0) {
					if (!server->handleCommand(connSock, buf)) {
						log->warn("Failed to successfully handle data");
					}
				}
				else {
					close(connSock);

					/* Remove connection record in server */
					if (!server->removeConnection(connSock)) {
						log->warn("Failed remove connection");
					}

					/* Dencrease event count */
					struct epoll_event *tmpEvents = new epoll_event[numEvents-1];
					if (tmpEvents == nullptr) {
						log->warn("Failed to allocate memory for events");
						break;
					}
					delete[] events;
					events = tmpEvents;
					numEvents--;

					log->info("Removed socket : %d", connSock);
				}
			}
		}
	}

	delete server;
}
