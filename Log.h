#ifndef __LOG_H__
#define __LOG_H__

#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <stdio.h>

using namespace std;

class Log {
	public:
		static Log* getInstance() {
			if (inst_ == nullptr) {
				char path[128];
				int len;

				/* Get the program name */
				if ((len = readlink("/proc/self/exe", path, sizeof(path))) == -1) {
					return nullptr;
				}

				if (len < sizeof(path)) { // Add terminating character
					path[len] = '\0';
				}

				/* Allocate memory for name */
				progname = new char[64];

				strncpy(progname, basename(path), 64);

				inst_ = new Log(progname);
			}
			return inst_;
		}

		void debug(const char *msg, ...) {
			char buf[512];
			va_list args;
			va_start(args, msg);
			vsnprintf(buf, sizeof(buf), msg, args);
			syslog(LOG_DEBUG, buf);
			va_end(args);
		}

		void info(const char *msg, ...) {
			char buf[512];
			va_list args;
			va_start(args, msg);
			vsnprintf(buf, sizeof(buf), msg, args);
			syslog(LOG_INFO, buf);
			va_end(args);
		}

		void notice(const char *msg, ...) {
			char buf[512];
			va_list args;
			va_start(args, msg);
			vsnprintf(buf, sizeof(buf), msg, args);
			syslog(LOG_NOTICE, buf);
			va_end(args);
		}

		void warn(const char *msg, ...) {
			char buf[512];
			va_list args;
			va_start(args, msg);
			vsnprintf(buf, sizeof(buf), msg, args);
			syslog(LOG_WARNING, buf);
			va_end(args);
		}

		void err(const char *msg, ...) {
			char buf[512];
			va_list args;
			va_start(args, msg);
			vsnprintf(buf, sizeof(buf), msg, args);
			syslog(LOG_ERR, buf);
			va_end(args);
		}

		void crit(const char *msg, ...) {
			char buf[512];
			va_list args;
			va_start(args, msg);
			vsnprintf(buf, sizeof(buf), msg, args);
			syslog(LOG_CRIT, buf);
			va_end(args);
		}

		void alert(const char *msg, ...) {
			char buf[512];
			va_list args;
			va_start(args, msg);
			vsnprintf(buf, sizeof(buf), msg, args);
			syslog(LOG_ALERT, buf);
			va_end(args);
		}

		void emerg(const char *msg, ...) {
			char buf[512];
			va_list args;
			va_start(args, msg);
			vsnprintf(buf, sizeof(buf), msg, args);
			syslog(LOG_EMERG, buf);
			va_end(args);
		}


	~Log() {
		delete inst_;
		delete progname;
	}

	private:
		static Log *inst_;
		static char *progname;

		Log(const char *name) {
			openlog(name, LOG_NDELAY | LOG_PID, LOG_USER);
			info("Logging initialized");
		}

};

#endif
