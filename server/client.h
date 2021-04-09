#pragma once

#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include "myservice.grpc.pb.h"
#include "logger_syslog.h"

using grpc::Channel;
using namespace ffsrv;

namespace ffsrv 
{
    class Client
    {
    public:

        Client();
        ~Client();

         /**
         * Server endpoint
         */ 
        struct Server
        {
            std::string hostname;
            int         port;
            std::string address() const;
        };
        typedef std::vector<Server> Servers;

        struct InitialisationParameters
        {
            std::string ca;
            std::string key;
            std::string cert;
            Servers     servers;
        };
        bool initialise(const InitialisationParameters& parameters);

        struct LoginParameters
        {
            std::string caller_identity;
        };
        bool login(const LoginParameters&);
        bool logout();
        bool addLocation(double x, double y, double z, const std::string& userData);

    private:

        /**
         * Method enum for logging purposes
         */ 
        enum Method
        {
            Login = 1,
            Logout,
            AddLocation
        };

        /**
         * Errors returned by client
         */ 
        enum Error
        {
            Success   = 0,
            NoBroker  = 1,
            RPCFailed = 2
        };

        // rpc
        typedef std::unique_ptr<MyService::Stub> StubPtr;
        std::vector<StubPtr> stubs_;
        MyService::Stub* get_stub() const;
        std::vector< std::shared_ptr<Channel> > channels_;

        // hidden
        Client(const Client&) = delete;
        Client& operator = (const Client&) = delete;
    
        // logging
        std::unique_ptr<Logger> logger_;
        void log(Method m, Error e, const std::string& msg, Logger::LogSeverity severity);
        void log(Method m, Error e, Logger::LogSeverity severity);

        // retry function call decorator
        template<class TFunc, class ... TArgs>
        bool make_call(TFunc&& fn, TArgs&& ... args)
        {
            bool succeeded = false;
            static const uint8_t MAX_RETRIES = 3;
            uint8_t retries = 0;
            while(!succeeded && retries < MAX_RETRIES)
            {
                succeeded = fn(std::forward<TArgs>(args)...);
                retries++;
            }
            return succeeded;
        }

        std::string getCallerIdentity();
        uint64_t    session_;
    };
};
