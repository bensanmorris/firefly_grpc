#include <sstream>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>

#include "myservice.grpc.pb.h"
#include "client.h"
#include "logger.h"
#include "utils.h"

using grpc::ClientContext;
using grpc::Status;

namespace ffsrv 
{
    Client::Client() : logger_(new SysLogLogger()), session_(0)
    {}
    
    std::string Client::Server::address() const
    {
        std::stringstream a;
        a << hostname << ":" << port;
        return a.str();
    }

    bool Client::initialise(const InitialisationParameters& params)
    {
        // parse args
        std::string ca_str;
        read(params.ca, ca_str);
        if(ca_str.empty())
            return false;

        std::string key_str;
        read(params.key, key_str);
        if(key_str.empty())
            return false;
 
        std::string cert_str;
        read(params.cert, cert_str);
        if(cert_str.empty())
            return false;

        // ssl options
        grpc::SslCredentialsOptions ssl_options = { ca_str, key_str, cert_str };
    
        // channels
        for(const auto & server : params.servers)
        {
            channels_.push_back(grpc::CreateChannel(server.address(), grpc::SslCredentials(ssl_options)));
            stubs_.push_back(MyService::NewStub(channels_[channels_.size()-1]));
        }
        return true;
    }

    Client::~Client()
    {}

    bool Client::login(const LoginParameters& v)
    {
        auto fn = [&]() {
            MyService::Stub* stub = get_stub();
            if(!stub)
            {
                log(Method::Login, Error::NoBroker, Logger::LogSeverity::Error);
                return false;
            }
            LoginRequest request;
            std::string callerIdentity = getCallerIdentity();
            request.set_caller_identity(callerIdentity);
            LoginResponse reply;
            ClientContext context;
            Status status = stub->Login(&context, request, &reply);
            if(status.ok()) 
            {
                log(Method::Login, Error::Success, Logger::LogSeverity::Normal);
                session_ = reply.session_handle();
                return true;
            }
            log(Method::Login, Error::RPCFailed, status.error_message(), Logger::LogSeverity::Error);
            return false;
        };
        return make_call(fn);
    }

    bool Client::logout()
    {
        auto fn = [&]() {
            MyService::Stub* stub = get_stub();
            if(!stub)
            {
                log(Method::Logout, Error::NoBroker, Logger::LogSeverity::Error);
                return false;
            }
            LogoutRequest request;
            request.set_session_handle(session_);
            LogoutResponse reply;
            ClientContext context;
            Status status = stub->Logout(&context, request, &reply);
            if(status.ok()) 
            {
                log(Method::Logout, Error::Success, Logger::LogSeverity::Normal);
                session_ = 0;
                return true;
            }
            log(Method::Logout, Error::RPCFailed, status.error_message(), Logger::LogSeverity::Error);
            return false;
        };
        return make_call(fn);
    }

    bool Client::addLocation(double x, double y, double z, const std::string& userData)
    {
        auto fn = [&]() {
            MyService::Stub* stub = get_stub();
            if(!stub)
            {
                log(Method::AddLocation, Error::NoBroker, Logger::LogSeverity::Error);
                return false;
            }
            AddLocationRequest request;
            request.set_session_handle(session_);
            Location* loc = request.mutable_location();
            loc->set_x(x);
            loc->set_y(y);
            loc->set_z(z);
            loc->set_user_data(userData);
            AddLocationResponse reply;
            ClientContext context;
            Status status = stub->AddLocation(&context, request, &reply);
            if(status.ok()) 
            {
                log(Method::AddLocation, Error::Success, Logger::LogSeverity::Normal);
                return true;
            }
            log(Method::AddLocation, Error::RPCFailed, status.error_message(), Logger::LogSeverity::Error);
            return false;
        };
        return make_call(fn);
    }

    MyService::Stub* Client::get_stub() const
    {
        if(stubs_.empty())
            return nullptr;
        return stubs_[rand() % stubs_.size()].get();
    }

    void Client::log(Method m, Error e, const std::string& msg, Logger::LogSeverity severity)
    {
        if(msg.empty())
            logger_->log("Method:%d -> E:%d", severity, m, e);
        else
            logger_->log("Method:%d -> E:%d -> Msg:%s", severity, m, e, msg.c_str());
    }

    void Client::log(Method m, Error e, Logger::LogSeverity severity)
    {
        log(m, e, "", severity);
    }

    std::string Client::getCallerIdentity()
    {
        return "alice"; // TODO
    }
}
