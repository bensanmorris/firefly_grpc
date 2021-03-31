#pragma once

#include <ctime>
#include <string>
#include <vector>

namespace soci
{
    class soci_error;
};

namespace ffsrv 
{
    class Logger;
    class DBAccess
    {
    public:

        /**
         * Database connection parameters
         */   
        struct ConnectionParameters
        {
            ConnectionParameters(const std::string& dbhostName, uint32_t dbport, const std::string& dbname, const std::string& dbuname, const std::string& dbpword, const std::string& dbsslCA, const std::string& dbsslCert) 
            : 
                hostName(dbhostName),
                port(dbport),
                name(dbname),
                userName(dbuname), 
                password(dbpword),
                sslCA(dbsslCA),
                sslCert(dbsslCert)
            {}
            std::string hostName;
            uint32_t    port;
            std::string name;
            std::string userName;
            std::string password;
            std::string sslCA;
            std::string sslCert;
        };

        /**
         * Database access initialisation
         */ 
        DBAccess(Logger&, const ConnectionParameters&);
        ~DBAccess();

        /**
         * Session
         */ 
        struct Session
        {
            std::string caller_identity;
            uint64_t handle;
            Session() : handle(0) {}
            std::tm created_timestamp;
        };
        bool createSession(Session&);
        bool deleteSession(uint64_t session_handle);
        bool readSession(Session&); 

        /**
         * Location 
         */
        struct Location
        {
            double x;
            double y;
            double z;
            std::string user_data;
            Location(double _x, double _y, double _z, const std::string& ud) : x(_x), y(_y), z(_z), user_data(ud){}
        };
        bool addLocation(uint64_t session, const Location&);

    private:
        
        std::string getConnectionString() const;
        Logger& logger_;
        ConnectionParameters connectionParameters_;
    };
}
