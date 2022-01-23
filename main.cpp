#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <new>
#include "Log.h"
#include "Server.h"

Log* Log::inst_ = nullptr;
char* Log::progname = nullptr;

int main(int argc, char **argv)
{
	Log *log = Log::getInstance();
	Server *server = new Server();

	log->info("Start up");

	server->initialize();


}
