#include <stdio.h>
#include "log.h"
#include "date.h"

bool LogCreated = false;

int createLog(void) {
	if (!LogCreated){
		FILE* log = fopen(LOG_PATH, "w");
		if (!log){
			return -1;
		}
		LogCreated = true;
		Log("************** Log file created at ");
		Log(RetTime(true).c_str());
		Log(" **************");
		Log("\n");
		fclose(log);
	}
	return 0;
}

void Log(const char *message) {
	FILE *log;
 
	if (!LogCreated) {
		createLog();
	}
	
	log = fopen(LOG_PATH, "a");

	if (!log) {
		if (LogCreated) {
			LogCreated = false;
		}
		return;
	} else {
		fputs(message, log);
		fclose(log);
	} 
}

void LogFM(const char *from, const char *message) {
	FILE *log;

	if (!LogCreated) {
		createLog();
	}
	
	log = fopen(LOG_PATH, "a");
	
	if (!log) {
		if (LogCreated) {
			LogCreated = false;
		}
		return;
	} else {
		fputs("Method: <", log);
		fputs(from, log);
		fputs("> :\r\t\tMessage:<", log);
		fputs(message, log);
		fputs(">", log);
		fputs("\n", log);
		fclose(log);
	}
}

void LogFMA(const char *from, const char *message, const char *additional_info) {
	FILE *log;

	if (!LogCreated) {
		createLog();
	}
	
	log = fopen(LOG_PATH, "a");

	if (!log) {
		if (LogCreated) {
			LogCreated = false;
		}
		return;
	} else {
		fputs("Method: <", log);
		fputs(from, log);
		fputs("> :\r\t\tMessage: <", log);
		fputs(message, log);
		fputs("> \tAdditional info:<", log);
		fputs(additional_info, log);		
		fputs(">", log);
		fputs("\n", log);
		fclose(log);
	} 
}
