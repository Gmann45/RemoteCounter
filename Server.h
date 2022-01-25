#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/epoll.h>
#include "Log.h"
#include "Counter.h"

using namespace std;

#define SERVER__PORT			8089

class Server {
	public:
		Counter *counter;
		bool init(int port = SERVER__PORT);
		void cleanUp(void);
		int getServerSock(void);
		int getMaxConnections(void);
		bool handleCommand(char *buf, int len);
		Server();
		~Server();
	private:
		Log *log;
		int serverSock;
		const int MAX_CONNECTIONS = 1024;
};

#endif
