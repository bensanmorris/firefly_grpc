#include "server.h"

#include <chrono>
#include <functional>
#include <inttypes.h>

#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include "soci/soci.h"

#include "utils.h"

using namespace ffsrv;
using namespace std::chrono;

// TODO - probably want to store this data in the database
#define AUDIT_METHOD(context, session, method_name) \
        std::stringstream ss; \
        ss << "Method = " << method_name; \
        ss << ";client_metadata = "; \
        std::multimap<grpc::string_ref, grpc::string_ref> client_metadata = context->client_metadata(); \
        for(auto s : client_metadata) \
            ss << s.first << " = " << s.second << ","; \
        std::vector<grpc::string_ref> peer_identity = context->auth_context()->GetPeerIdentity(); \
        ss << ";peer_identity = "; \
        for(auto s : peer_identity) \
            ss << s << ","; \
        ss << ";peer_identity_property_name = " << context->auth_context()->GetPeerIdentityPropertyName() << std::endl; \
        std::vector<grpc::string_ref> subject = context->auth_context()->FindPropertyValues(GRPC_X509_CN_PROPERTY_NAME); \
        ss << ";subject = "; \
        for(auto p : subject) \
            ss << p << ","; \
        audit(session, ss.str()); \
        if(!validate_session(session)) \
            return Status::CANCELLED;

namespace ffsrv 
{
    ServerService::ServerService() 
    : 
        Service<MyService::Service>(true /* provide db options */)
    {
        db_timer_.add(CppTime::clock::now(), [this](CppTime::timer_id timer_id){
            ping(timer_id);
        }, seconds(3));
    }

    void ServerService::declareOptions()
    {
        Service::declareOptions();
    }

    bool ServerService::configure()
    {
        if(!Service::configure())
            return false;

        return true;
    }

    void ServerService::run()
    {
        std::stringstream server_address;
        server_address << interface_ << ":" << port_;
        grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = { key_str_, cert_str_ };
        grpc::ServerBuilder builder;
        grpc::SslServerCredentialsOptions ssl_options(GRPC_SSL_REQUEST_CLIENT_CERTIFICATE_AND_VERIFY);
        ssl_options.pem_root_certs = ca_str_;
        ssl_options.pem_key_cert_pairs.push_back(keycert);
        builder.AddListeningPort(server_address.str(), grpc::SslServerCredentials(ssl_options));
        builder.RegisterService(this);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        std::cout << "Server listening on " << server_address.str() << std::endl;
        server->Wait();
    }

    void ServerService::ping(CppTime::timer_id)
    {
        // TODO - update db with server's status   
    }

    Status ServerService::Login(ServerContext* context, const LoginRequest* request, LoginResponse* reply) 
    {
        AUDIT_METHOD(context, 0 /* Session Id not set yet  */, "Login")
        const std::string& caller = request->caller_identity();
        if(!validate_caller(caller))
            return Status::CANCELLED;

        DBAccess::Session session;
        session.caller_identity = caller;
        if(!db().createSession(session) || session.handle == 0)
        {
            log_error(LoginFailed, "Failed to create session for caller:%s", caller.c_str());
            return Status::CANCELLED;
        }
        reply->set_session_handle(session.handle);
        audit(session.handle, "Login"); 
        return Status::OK;
    }

    Status ServerService::Logout(ServerContext* context, const LogoutRequest* request, LogoutResponse* reply) 
    {
        AUDIT_METHOD(context, request->session_handle(), "Logout")
        if(!db().deleteSession(request->session_handle()))
        {
            log_error(LogoutFailed, "Failed to delete session:%" PRIu64, request->session_handle());
            return Status::CANCELLED;
        }
        return Status::OK;
    }
    
    Status ServerService::AddLocation(ServerContext* context, const AddLocationRequest* request, AddLocationResponse* reply) 
    {
        AUDIT_METHOD(context, request->session_handle(), "AddLocation")
        if(!validate_session(request->session_handle()))
        {
            log_error(AddLocationFailed, "Failed to validate session:%" PRIu64, request->session_handle());
            return Status::CANCELLED;
        }
        const Location& location = request->location();
        DBAccess::Location loc(location.x(), location.y(), location.z(), location.user_data());
        if(!db().addLocation(request->session_handle(), loc))
        {
            log_error(AddLocationFailed, "Failed to add location for session:%" PRIu64, request->session_handle());
            return Status::CANCELLED;
        }
        return Status::OK;
    }
    
    bool ServerService::validate_caller(const std::string& caller)
    {
        // TODO
        return true;
    }
    
    bool ServerService::validate_session(uint64_t session)
    {
        // TODO
        return true;
    }

    DBAccess ServerService::db()
    {
        // TODO: may want to cache
        DBAccess db(*logger_, DBAccess::ConnectionParameters{
            db_hostname_,
            db_port_,
            db_name_,
            db_username_,
            db_password_,
            db_ca_,
            db_cert_ 
        });
        return db;
    }
}
