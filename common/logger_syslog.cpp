#include "logger_syslog.h"

namespace ffsrv 
{

    void logRecordFormatter(logging::record_view const& rec, logging::formatting_ostream& strm)
    {
        // see here: www.boost.org/doc/libs/1_55_0/libs/log/doc/html/log/detailed.htm
        strm << "(" << logging::extract< std::string >("BindAddress", rec) << ")"
             << "<" << logging::extract< Logger::LogSeverity >("Severity", rec) << "> - "
             << rec[expr::smessage];
    }

    SysLogLogger::SysLogLogger() 
    :
        isGood_(false),
        sink_(boost::make_shared<LoggingBackend>(keywords::use_impl = sinks::syslog::native, keywords::facility = sinks::syslog::local7)),
        logger_(keywords::severity = LogSeverity::Normal)
    {
    }

    SysLogLogger::~SysLogLogger()
    {}

    void SysLogLogger::init()
    {
        try
        {
            // custom log formatter routine
            sink_->set_formatter(&logRecordFormatter);
            
            // map our custom levels to the syslog levels
            sinks::syslog::custom_severity_mapping< LogSeverity > mapping("Severity");
            mapping[LogSeverity::Normal]  = sinks::syslog::info;
            mapping[LogSeverity::Warning] = sinks::syslog::warning;
            mapping[LogSeverity::Error]   = sinks::syslog::critical;
            
            // configure our log severity mappings
            sink_->locked_backend()->set_severity_mapper(mapping);

            // add the sink to the core
            logging::core::get()->add_sink(sink_);

            // add some attributes too
            logging::add_common_attributes();
        }
        catch(...)
        {
            isGood_ = false;
            log("An error occurred whilst initialising the logging", LogSeverity::Error);
        }
        isGood_ = true;
    } 

    void SysLogLogger::setBindAddress(const std::string& address)
    {
        bindAddress_ = address;
        logger_.add_attribute("BindAddress", attrs::constant< std::string >(bindAddress_));
    }

    void SysLogLogger::log(const std::runtime_error& e)
    {
        log("Error encountered: %s", LogSeverity::Error, e.what());
    }
    
    void SysLogLogger::log(const std::string& format, LogSeverity severity, ...)
    {
        va_list va;
        va_start(va, severity);
        static const int MAX_BUFFER_LEN = 1024;
        char buffer[MAX_BUFFER_LEN];
        vsnprintf(buffer, MAX_BUFFER_LEN, format.c_str(), va);
        if(isGood_)
            BOOST_LOG_SEV(logger_, severity) << buffer;
        else
            std::cout << buffer << std::endl;
#ifdef TRACE_TO_STD_OUT
        std::cout << buffer << std::endl;
#endif
        va_end(va);
    }
}
