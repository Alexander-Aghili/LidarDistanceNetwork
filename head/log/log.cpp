
#include <iostream>
#include <fstream>
#include "log.h"

std::string mainLog = "log.txt";
std::string verboseLog = "verbose_log.txt";

/* Start log message with date/time logging */
void startLog() {
	startLogMessage(mainLog);
	startLogMessage(verboseLog);
}

void startLogMessage(std::string logFile) {
	std::ofstream file(logFile.c_str());
	time_t now = time(0);
	tm *ltm = localtime(&now);
	file << "Log Start: " << 
		ltm->tm_mday << "/" << 1 + ltm->tm_mon << "/" << 1900 + ltm->tm_year << " " << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << std::endl;
	file.close();
}

/* Recieves a string, get the current time and appends the log file to add message and then time in getTime() format */
//@Param string: log
void writeToLog(std::string log) {
	std::ofstream file;
	file.open(mainLog, std::ios_base::out | std::ios_base::app);
	if (file.is_open())
		file << log + " " + getTime() << std::endl;
	
	file.close();
}

void writeToLog(std::string log, bool verboseOnly) {
	std::ofstream file;
	file.open(verboseLog, std::ios_base::out | std::ios_base::app);
	if (file.is_open())
		file << log + " " + getTime() << std::endl;
	file.close();

	if (!verboseOnly) {
		writeToLog(log);
	}

}

/* returns time in string form, formatted as Hours:Minutes:Seconds */
//@Return: string: time
std::string getTime() {
	time_t now = time(0);
	tm *ltm = localtime(&now);
	std::string time = std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) +  ":" + std::to_string(ltm->tm_sec);
	return time;
}