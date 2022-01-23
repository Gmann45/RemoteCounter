#ifndef __SERVER_H__
#define __SERVER_H__

#include "Log.h"
#include "Counter.h"

using namespace std;

#define SERVER__MAX_CONNECTIONS	1024
#define SERVER__PORT			8089

class Server {
	public:
		Counter *counter;
		bool initialize(int port = SERVER__PORT);
		void cleanUp(void);
		Server();
		~Server();
	private:
		Log *log;
		int serverSock;
		int connSock;
		
};

#endif
