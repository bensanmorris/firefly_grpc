#include <iostream>
#include <sstream>
#include "soci/soci.h"
#include "dbaccess.h"
#include "logger.h"

using namespace soci;

#define HANDLE_EXCEPTION(e) \
    catch(const std::exception& e)\
    {\
        logger_.log(e.what(), Logger::LogSeverity::Error);\
    }\

namespace ffsrv 
{
    DBAccess::DBAccess(Logger& logger, const ConnectionParameters& connectionParameters)
    :
        logger_(logger),
        connectionParameters_(connectionParameters)
    {
    }

    DBAccess::~DBAccess()
    {}

    std::string DBAccess::getConnectionString() const
    {
        std::stringstream connStr;
        connStr << "mysql://host="  << connectionParameters_.hostName;
        connStr << " port="         << connectionParameters_.port;
        connStr << " db="           << connectionParameters_.name;
        connStr << " user="         << connectionParameters_.userName;
        connStr << " password='"    << connectionParameters_.password << "'"; 
        if(!connectionParameters_.sslCA.empty())
            connStr << " sslca=" << connectionParameters_.sslCA;
        if(!connectionParameters_.sslCert.empty()) 
            connStr << " sslcert=" << connectionParameters_.sslCert;
        return connStr.str();
    }

    bool DBAccess::createSession(Session& session_)
    {
        session_.handle = 0;
        try
        {
            session sql(getConnectionString());
            std::stringstream sqlStr;
            sqlStr << "insert into " << connectionParameters_.name  << ".session (caller_identity) values(:ci)";
            statement st = (sql.prepare << sqlStr.str(), use(session_.caller_identity, "ci"));
            st.execute(true);
            if(st.get_affected_rows() != 1)
            {
                logger_.log("Failed to insert session", Logger::LogSeverity::Error);
                return false;
            }
            sql.get_last_insert_id("session", (long long&)session_.handle);
            if(session_.handle == 0)
            {
                logger_.log("Failed to insert session - couldn't obtain last inserted id", Logger::LogSeverity::Error);
                return false;
            }
            return true;
        }
        HANDLE_EXCEPTION(e) 
        return false;
    }
        
    bool DBAccess::deleteSession(uint64_t session_handle)
    {
        try
        {
            session sql(getConnectionString());
            std::stringstream sqlStr;
            sqlStr << "delete from " << connectionParameters_.name <<  ".session where session_id = :sid";
            statement st = (sql.prepare << sqlStr.str(), use(session_handle, "sid"));
            st.execute(true);
            if(st.get_affected_rows() != 1)
            {
                logger_.log("Failed to delete session : %d", Logger::LogSeverity::Error, session_handle);
                return false;
            }
            return true;
        }
        HANDLE_EXCEPTION(e) 
        return false;
    }

    bool DBAccess::readSession(Session& session_)
    {
        try
        {
            session sql(getConnectionString());
            std::stringstream sqlStr;
            sqlStr << "select created_timestamp from " << connectionParameters_.name << ".session where session_id = :sid";
            statement st = (sql.prepare << sqlStr.str(), into(session_.created_timestamp), use(session_.handle, "sid"));
            if(!st.execute(true) || st.get_affected_rows() != 1)
            {
                logger_.log("Failed to obtain session with session_id: %d", Logger::LogSeverity::Error, session_.handle);
                return false;
            }
            return true;
        }
        HANDLE_EXCEPTION(e) 
        return false;
    }

    bool DBAccess::addLocation(uint64_t session_id, const Location& location)
    {
        try
        {
            session sql(getConnectionString());
            std::stringstream sqlStr;
            sqlStr << "insert into " << connectionParameters_.name  << ".location (session_id, x, y, z, user_data) values(:s, :x, :y, :z, :ud)";
            statement st = (sql.prepare << sqlStr.str(), use(session_id, "s"), use(location.x, "x"), use(location.y, "y"), use(location.z, "z"), use(location.user_data, "ud"));
            st.execute(true);
            if(st.get_affected_rows() != 1)
            {   
                logger_.log("Failed to insert location", Logger::LogSeverity::Error);
                return false;
            }
            return true;
        }
        HANDLE_EXCEPTION(e)
        return false;
    }
}
