#ifndef LOG_H
#define LOG_H

#include <iostream>

void startLog();
void startLogMessage(std::string logFile);
void writeToLog(std::string log); 
void writeToLog(std::string log, bool verboseOnly);
std::string getTime(); //String in format Hours:Minutes:Seconds

#endif