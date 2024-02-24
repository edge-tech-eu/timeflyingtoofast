#ifndef LOGGING_H
#define LOGGING_H

#define LOG_ON          true
#define LOG_OFF         false
#define LOG_ON_DEBUG    true
#define LOG_ON_INFO     false

void logging_init(void);
void logging_serial2(bool logon, bool debug);
void logging_remote(bool logon, bool debug);

#endif