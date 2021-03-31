#pragma once

#include <string>

#include "logger.h"

#define BOOST_LOG_USE_NATIVE_SYSLOG
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

namespace ffsrv 
{
    class SysLogLogger : public Logger
    {
    public:
        SysLogLogger();
        virtual ~SysLogLogger();
        virtual void init();
        virtual void setBindAddress(const std::string&);        

        virtual void log(const std::string&, LogSeverity severity, ...);
        virtual void log(const std::runtime_error&);

    private:

        std::string bindAddress_;
        bool isGood_;
        typedef sinks::synchronous_sink< sinks::syslog_backend > LoggingBackend;
        boost::shared_ptr<LoggingBackend> sink_;
        src::severity_logger_mt< LogSeverity > logger_;
    };
}
