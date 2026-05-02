#pragma once

#include <cstdint>
#include <string>
#include <sstream>

namespace server{
namespace basic{

enum class LOG_LEVEL {
    FATAL, 
    ERROR,
    WARNING,
    INFO, 
    DEBUG,
    TRACE
};

class Logger {
public:
    static LOG_LEVEL level;
    static void write(
        const char* filename,
        int line,
        LOG_LEVEL msglevel,
        std::string msg
    );
    static void setLogLevel(const std::string& debugString);
};

class LogStream {
public:

    LogStream(
        const char *filename,
        int line,
        LOG_LEVEL level
    );

    ~LogStream();

    template<typename T>
    LogStream& operator<<(const T& log){
        stream_ << log;
        return *this;
    }
    
private:
    LOG_LEVEL level_;
    std::ostringstream stream_;
    const char *filename_;
    uint32_t line_;
};


}
}

#define LOG_FATAL       server::basic::LogStream(__FILE__, __LINE__, server::basic::LOG_LEVEL::FATAL)
#define LOG_ERROR       server::basic::LogStream(__FILE__, __LINE__, server::basic::LOG_LEVEL::ERROR)
#define LOG_WARNING     server::basic::LogStream(__FILE__, __LINE__, server::basic::LOG_LEVEL::WARNING)
#define LOG_INFO        server::basic::LogStream(__FILE__, __LINE__, server::basic::LOG_LEVEL::INFO)
#define LOG_DEBUG       server::basic::LogStream(__FILE__, __LINE__, server::basic::LOG_LEVEL::DEBUG)
#define LOG_TRACE       server::basic::LogStream(__FILE__, __LINE__, server::basic::LOG_LEVEL::TRACE)

