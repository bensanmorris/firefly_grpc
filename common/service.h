#pragma once

#include <inttypes.h>
#include <memory>
#include <string>
#include <sstream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "logger_syslog.h"
#include "utils.h"

namespace ffsrv 
{
    enum ServerError
    {
        ConfigurationError = 1,
        Success,
        FatalError,
        CAFileError,
        KeyFileError,
        CertificateFileError,
        MaxError    
    };

    template<typename T>
    class Service : public T
    {
    public:
            
        Service(bool db_params_enabled = false);
        int start(int argc, char* argv[]);
        
        template <class ... Types>
        void log(const std::string& format, Logger::LogSeverity severity, Types... args)
        {
            logger_->log(format, severity, args...);
        }
        template <class ... Types>
        void log_error(uint8_t error_code, const std::string& format, Types... args)
        {
            std::stringstream ss;
            ss << "E" << error_code << ":" << format;
            logger_->log(ss.str(), Logger::LogSeverity::Error, args...);
        }
        void log_error(uint8_t error_code, const std::exception& e)
        {
            log_error(error_code, e.what());
        }
        template <class ... Types>
        void log_info(const std::string& format, Types... args)
        {
            logger_->log(format, Logger::LogSeverity::Normal, args...);
        }
        void audit(const std::string& who, const std::string& what)
        {
            log_info("who = %s, %s", who.c_str(), what.c_str());
        }
        void audit(uint64_t who, const std::string& what)
        {
            log_info("who = %" PRIu64  ", %s", who, what.c_str());
        }
    protected:

        virtual void declareOptions();
        virtual bool configure();
        virtual void run();
        std::string getConnectionString() const;

        // server
        std::string interface_;
        uint32_t    port_;
        std::string ca_;
        std::string ca_str_;
        std::string key_;
        std::string key_str_;
        std::string cert_;
        std::string cert_str_;
        
        // db (optional)
        bool        db_params_enabled_;
        std::string db_hostname_;
        uint32_t    db_port_;
        std::string db_ca_;
        std::string db_cert_;
        bool        db_verify_cert_;
        std::string db_name_;
        std::string db_username_;
        std::string db_password_;

    protected:

        po::options_description programOptions_;
        po::variables_map programOptionsValues_;
        std::shared_ptr<Logger> logger_;
    };

    template<typename T>
    Service<T>::Service(bool db_params_enabled) : 
        programOptions_("Allowed options"),
        logger_(std::make_shared<SysLogLogger>(SysLogLogger())),
        db_params_enabled_(db_params_enabled)
    {
    }

    template<typename T>    
    int Service<T>::start(int argc, char* argv[])
    {
        try
        {
            // initialise logging
            logger_->init();

            // grab command line args / configure
            declareOptions();
            po::store(po::parse_command_line(argc, argv, programOptions_), programOptionsValues_);
            po::notify(programOptionsValues_);
            if(!configure())
                return 1; 

            // now we have the server's address and port pass it to the logger so we can include it in each log entry 
            std::stringstream bindAddress;
            bindAddress << interface_ << "." << port_; 
            logger_->setBindAddress(bindAddress.str()); 

            // go!
            run();
        }
        catch(const std::exception& e)
        {
            std::stringstream err;
            err << "A fatal error occurred in the server with args: ";
            for(int i = 0; i < argc; ++i)
                err << argv[i] << ",";
            err << ". Error is: " << e.what();
            std::cout << err.str() << std::endl;
            log_error(FatalError, err.str());
            std::cout << programOptions_ << std::endl;
            return 2;
        }
        catch(...)
        {
            std::stringstream err;
            err << "An unexpected fatal error occurred in the server with args: ";
            for(int i = 0; i < argc; ++i)
                err << argv[i] << ",";
            std::cout << err.str() << std::endl;
            log_error(FatalError, err.str());
            return 3;
        }
        return 0;
    }

    template<typename T>    
    void Service<T>::declareOptions()
    {
        programOptions_.add_options()
            ("help", "produce help message")
            ("address", po::value<std::string>()->default_value("0.0.0.0"), "the bind address")
            ("port",    po::value<uint32_t>()->default_value(50001),        "the port the server listens on")
            ("ca",      po::value<std::string>()->required(),               "the server certificate authority")
            ("key",     po::value<std::string>()->required(),               "the server private key")
            ("cert",    po::value<std::string>()->required(),               "the server certificate")
        ;
        if(db_params_enabled_)
        {
            programOptions_.add_options()
                ("db_hostname",     po::value<std::string>()->default_value(""),                "the db host name")
                ("db_port",         po::value<uint32_t>()->default_value(3306),                 "the db port")
                ("db_ca",           po::value<std::string>()->default_value(""),                "the db certificate authority")
                ("db_cert",         po::value<std::string>()->default_value(""),                "the db certificate")
                ("db_verify_cert",  po::value<bool>()->default_value(false),                    "db should verify client certificate")
                ("db_name",         po::value<std::string>()->default_value(""),    "the db name")
                ("db_username",     po::value<std::string>()->default_value(""),                "the db user name")
                ("db_password",     po::value<std::string>()->default_value(""),                "the db password")
            ;
        }
    }
    
    template<typename T>
    bool Service<T>::configure()
    {
        if(programOptionsValues_.count("help"))
        {
            std::cout << programOptions_ << std::endl;
            return false;
        }

        interface_ = programOptionsValues_["address"].template as<std::string>();
      
        port_ = programOptionsValues_["port"].template as<uint32_t>();
 
        ca_ = programOptionsValues_["ca"].template as<std::string>();
        read(ca_, ca_str_); 
        if(ca_str_.empty())
        {
            log_error(CAFileError, "Failed to read ca file");
            return false;
        }
        
        key_ = programOptionsValues_["key"].template as<std::string>();
        read(key_, key_str_); 
        if(key_str_.empty())
        {
            log_error(KeyFileError, "Failed to read private key file");
            return false;
        }
        
        cert_ = programOptionsValues_["cert"].template as<std::string>();
        read(cert_, cert_str_); 
        if(cert_str_.empty())
        {
            log_error(CertificateFileError, "Failed to read certificate file");
            return false;
        }

        if(db_params_enabled_)
        {
            db_hostname_    = programOptionsValues_["db_hostname"].template as<std::string>();
            db_port_        = programOptionsValues_["db_port"].template as<uint32_t>();
            db_ca_          = programOptionsValues_["db_ca"].template as<std::string>();
            db_cert_        = programOptionsValues_["db_cert"].template as<std::string>();
            db_verify_cert_ = programOptionsValues_["db_verify_cert"].template as<bool>();
            db_name_        = programOptionsValues_["db_name"].template as<std::string>();
            db_username_    = programOptionsValues_["db_username"].template as<std::string>();
            db_password_    = programOptionsValues_["db_password"].template as<std::string>();
        }

        return true;
    }

    template<typename T>
    void Service<T>::run()
    {}

    template<typename T>
    std::string Service<T>::getConnectionString() const
    {
        std::stringstream cn_str;
        cn_str << "mysql://";
        if(!db_hostname_.empty())
            cn_str << "host=" << db_hostname_ << " ";
        cn_str << "port=" << db_port_ << " ";
        cn_str << "db=" << db_name_ << " ";
        cn_str << "user=" << db_username_ << " "; 
        cn_str << "password='" << db_password_ << "' ";
        if(!db_ca_.empty())
            cn_str << "sslca=" << db_ca_ << " "; 
        if(!db_cert_.empty())
            cn_str << "sslcert=" << db_cert_ << " ";
        return cn_str.str();
    }
};
