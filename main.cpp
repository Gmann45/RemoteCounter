#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <new>
#include "Counter.h"
#include "Log.h"

int main(int argc, char **argv)
{
	Log *logger = Log::getInstance();
	Counter *count_p = new Counter(0);

	logger->info("test");

	closelog();
	delete count_p;

}
