#include <stdio.h>
#include "log.h"

bool LogCreated = true;	// It's already done by the GUI

void createLog(void) {
	if (!LogCreated){
		FILE* log = fopen(LOG_PATH, "w");
		LogCreated = true;
		Log("************** Log file created at ");
		Log("??:??");
		Log(" **************");
		Log("\n");
		fclose(log);
	}
}

void Log(const char *message) {
	FILE *log;
 
	if (!LogCreated) {
		log = fopen(LOG_PATH, "w");
		LogCreated = true;
	} else {
		log = fopen(LOG_PATH, "a");
	}

	if (log == NULL) {
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
		log = fopen(LOG_PATH, "w");
		LogCreated = true;
	} else {
		log = fopen(LOG_PATH, "a");
	}

	if (log == NULL) {
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
		log = fopen(LOG_PATH, "w");
		LogCreated = true;
	} else {
		log = fopen(LOG_PATH, "a");
	}

	if (log == NULL) {
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
