#include "Particle.h"
#include "RemoteLogger.h"

const uint16_t RemoteLogHandler::kLocalPort = 8888;

RemoteLogHandler::RemoteLogHandler(String host, uint16_t port, String app, LogLevel level, 
    const LogCategoryFilters &filters, String system) : LogHandler(level, filters), m_host(host), m_port(port), m_app(app),
                                       m_system(system), m_queue(nullptr), m_thread(nullptr)  {
    m_inited = false;

    os_queue_create(&m_queue, sizeof(void*), 10, nullptr);
    
    m_thread = new Thread("remotelog", run, this, OS_THREAD_PRIORITY_DEFAULT + 1);
    
    LogManager::instance()->addHandler(this);
}

IPAddress RemoteLogHandler::resolve(const char *host) {

    IPAddress adr = Network.resolve(host);

    return(adr);
}

void RemoteLogHandler::run(void* arg) {
    auto self = static_cast<RemoteLogHandler*>(arg);
    while (true) {
        String* str = nullptr;
        
        self->lazyInit();

        if(!os_queue_take(self->m_queue, (void*)&str, (self->m_inited?CONCURRENT_WAIT_FOREVER:1000), nullptr)) {

            if (self->m_inited) {
                self->m_udp.sendPacket(str->c_str(), str->length(), self->m_address, self->m_port);
            }

            if (str) {
                delete str;
            }
        }
    }
}

void RemoteLogHandler::log(String message) {
    //String time = Time.format(Time.now(), TIME_FORMAT_ISO8601_FULL);
    auto str = new String(String::format("[%s] [%s] %s", m_system.c_str(), m_app.c_str(), message.c_str()));
    if (str) {
        if (os_queue_put(m_queue, &str, 0, nullptr)) {
            // Failed to post
            delete str;
        }
    }
}

RemoteLogHandler::~RemoteLogHandler() {
    LogManager::instance()->removeHandler(this);
}

void RemoteLogHandler::lazyInit() {
    
    if(Particle.connected()) {

        if (!m_inited) {
            
            uint8_t ret = m_udp.begin(kLocalPort);
            m_inited = ret != 0;
        }

        if (!m_address) {
            m_address = resolve(m_host);
        }
    }
}

const char* RemoteLogHandler::extractFileName(const char *s) {
    const char *s1 = strrchr(s, '/');
    if (s1) {
        return s1 + 1;
    }
    return s;
}

const char* RemoteLogHandler::extractFuncName(const char *s, size_t *size) {
    const char *s1 = s;
    for (; *s; ++s) {
        if (*s == ' ') {
            s1 = s + 1; // Skip return type
        } else if (*s == '(') {
            break; // Skip argument types
        }
    }
    *size = s - s1;
    return s1;
}

void RemoteLogHandler::logMessage(const char *msg, LogLevel level, const char *category, const LogAttributes &attr) {

        if (!m_inited) {
            // Short circuit
            return;
        }

        String s;

        if (attr.has_time) {
            s.concat(String::format("%010u ", (unsigned)attr.time));
            //char timestring[15];
            //sprintf(timestring, "%010u ", (unsigned)attr.time);
            //s.concat(timestring);
        }

        if (category) {
            s.concat("[");
            s.concat(category);
            s.concat("] ");
        }

        // Source file
        if (attr.has_file) {
            s = extractFileName(attr.file); // Strip directory path
            s.concat(s); // File name
            if (attr.has_line) {
                s.concat(":");
                s.concat(String(attr.line)); // Line number
            }
            if (attr.has_function) {
                s.concat(", ");
            } else {
                s.concat(": ");
            }
        }

        // Function name
        if (attr.has_function) {
            size_t n = 0;
            s = extractFuncName(attr.function, &n); // Strip argument and return types
            s.concat(s);
            s.concat("(): ");
        }

        // Level
        s.concat(levelName(level));
        s.concat(": ");

        // Message
        if (msg) {
            s.concat(msg);
        }

        // Additional attributes
        if (attr.has_code || attr.has_details) {
            s.concat(" [");
            // Code
            if (attr.has_code) {
                s.concat(String::format("code = %p" , (intptr_t)attr.code));
            }
            // Details
            if (attr.has_details) {
                if (attr.has_code) {
                    s.concat(", ");
                }
                s.concat("details = ");
                s.concat(attr.details);
            }
            s.concat(']');
        }

        log(s);
}
