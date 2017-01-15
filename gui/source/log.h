/**

	This method is created by Jolty95 (umbjolt in gbatemp) to help debugging TWLoader
	Use:
		#int createLog() to start using it
		#void Log(const char *message) write in log file a simple message. Use again if you want to write a new line with \n
		#void LogFM(const char *from, const char *message) write in log file a message FROM a method. Usefull to check which method make an exception
		#void LogFMA(const char *from, const char *message, const char *additional_info) write in log file a message with additional info. Ex. write in log a return value. Remember to parse it as a string.

*/

#ifndef LOG_H
#define LOG_H

#define LOG_PATH "sdmc:/_nds/twloader/log.ini"

extern bool LogCreated;

int createLog(void);
void Log(const char *message);
void LogFM(const char *from, const char *message);
void LogFMA(const char *from, const char *message, const char *additional_info);
#endif // LOG_H
