#pragma once

#include <stdexcept>
#include <string>

namespace ffsrv 
{
    class Logger
    {
    public:
        virtual ~Logger() {}
        enum LogSeverity
        {
            Normal,
            Warning,
            Error
        };
        virtual void init(){};
        virtual void setBindAddress(const std::string&){};
        virtual void log(const std::string&, LogSeverity severity, ...) = 0;
        virtual void log(const std::runtime_error&) = 0;
    };
};
