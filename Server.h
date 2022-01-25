#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/epoll.h>
#include <vector>
#include <string>
#include "Log.h"
#include "Counter.h"

#define SERVER__PORT			8089

class Server {
	public:
		Counter *counter;
		bool init(int port = SERVER__PORT);
		void cleanUp(void);
		int getServerSock(void);
		int getMaxConnections(void);
		void addConnection(int sock);
		bool removeConnection(int sock);
		bool handleCommand(int sock, char *buf);
		Server();
		~Server();
	private:
		Log *log;
		int serverSock;
		std::vector<int> connections;
		const int MAX_CONNECTIONS = 1024;
		const std::string SERVER__CMD__INCR = "INCR";
		const std::string SERVER__CMD__DECR = "DECR";
		const std::string SERVER__CMD__OUTPUT = "OUTPUT";
};

#endif
