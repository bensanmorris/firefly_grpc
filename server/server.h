#pragma once

#include <memory>

#include "service.h"
#include "myservice.grpc.pb.h"
#include "dbaccess.h"

#include "cryptopp/cryptlib.h"
#include "../ext/cpptime/cpptime.h"

using grpc::ServerContext;
using grpc::Status;

namespace ffsrv
{
    /**
     * The Server service implementation class.
     */
    class ServerService : public Service<MyService::Service>
    {
    public:
        
        ServerService(); 

        DBAccess db();

    protected:
        
        virtual void declareOptions();
        virtual bool configure();
        virtual void run();

    private:

        enum Error
        {
	        LoginFailed,
            LogoutFailed,
            AddLocationFailed
        };

        Status Login(ServerContext* context, const LoginRequest* request, LoginResponse* reply) override;
        Status Logout(ServerContext* context, const LogoutRequest* request, LogoutResponse* reply) override;
        Status AddLocation(ServerContext* context, const AddLocationRequest* request, AddLocationResponse* reply) override; 

        void ping(CppTime::timer_id);
        bool validate_caller(const std::string& caller);
        bool validate_session(uint64_t session);

        CppTime::Timer db_timer_;
    };
}
