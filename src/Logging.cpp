#include "Particle.h"
#include "Logging.h"
#include "RemoteLogger.h"

// keep order of 'retained' variables
retained bool serial2LoggerEnabled = LOG_ON;
retained bool serial2LoggerDebugEnabled = LOG_ON_DEBUG;
retained bool remoteLoggerEnabled = LOG_OFF;
retained bool remoteLoggerDebugEnabled = LOG_ON_INFO;

RemoteLogHandler *remoteLogHandler = NULL;
Serial2LogHandler *serial2LogHandler = NULL;

void logging_init(void) {

    logging_serial2(serial2LoggerEnabled,serial2LoggerDebugEnabled);
    logging_remote(remoteLoggerEnabled,remoteLoggerDebugEnabled);
}

void logging_serial2(bool logon, bool debug) {

    auto logManager = LogManager::instance();

    if(serial2LogHandler!=NULL) {
    
        logManager->removeHandler(serial2LogHandler);

        delete serial2LogHandler;
    }

    Serial2LogHandler* newLogHandler = NULL;
    
    if(logon) {

        newLogHandler = new Serial2LogHandler(115200, (debug?LOG_LEVEL_ALL:LOG_LEVEL_INFO));

        logManager->addHandler(newLogHandler);
    }
    
    serial2LogHandler = newLogHandler;
}


void logging_remote(bool logon, bool debug) {

    auto logManager = LogManager::instance();

    if(remoteLogHandler!=NULL) {
    
        logManager->removeHandler(remoteLogHandler);

        delete remoteLogHandler;
    }

    RemoteLogHandler* newLogHandler = NULL;
    
    if(logon) {

        newLogHandler = new RemoteLogHandler("monitor.edgetech.nl", 8888, "site-sm", (debug?LOG_LEVEL_ALL:LOG_LEVEL_INFO));

        logManager->addHandler(newLogHandler);
    }
    
    remoteLogHandler = newLogHandler;
}