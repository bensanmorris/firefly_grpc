#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "gtest/gtest.h"
#include "utils.h"
#include "../common/dbaccess.h"
#include "../common/logger.h"

using namespace ;

class TestLogger : public Logger
{
public:
    virtual void log(const std::string& s, LogSeverity severity, ...)
    {
        std::cout << s << std::endl;
    }
    virtual void log(const std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
    }
};

TEST(test_db, test_create_session)
{
    TestLogger logger;
    DBAccess db(logger, DBAccess::ConnectionParameters{
            "127.0.0.1",
            3306,
            DB_NAME,
            "ha_user",
            "Test123!", // password
            "",         // CA
            ""          // cert
    });

    // create
    DBAccess::Session session;
    session.caller_identity = "test_user";
    ASSERT_TRUE(db.createSession(session));
    std::cout << "generated session: " << session.handle << std::endl;
    ASSERT_TRUE(session.handle > 0);

    // read
    DBAccess::Session read_session;
    read_session.handle = session.handle;
    ASSERT_TRUE(db.readSession(read_session));

    // delete
    ASSERT_TRUE(db.deleteSession(session.handle));
}

TEST(test_db, test_create_key)
{
    TestLogger logger;
    DBAccess db(logger, DBAccess::ConnectionParameters{
            "127.0.0.1",
            3306,
            DB_NAME,
            "ha_user",
            "Test123!", // password
            "",         // CA
            ""          // cert
    });

    // random key name generator (to avoid collision whilst re-testing)
    boost::uuids::uuid uuid = boost::uuids::random_generator()();

    // create
    DBAccess::Key key;
    key.keyDomain   = "TEST_DOMAIN";
    key.keyName     = boost::lexical_cast<std::string>(uuid);
    key.keyLength   = 2048;
    key.keyMaterial = "ABC123";
    ASSERT_TRUE(db.insertKey(key));

    // read
    DBAccess::Key key_verify;
    key_verify.keyDomain = key.keyDomain;
    key_verify.keyName   = key.keyName;
    ASSERT_TRUE(db.getKey(key_verify));
    ASSERT_TRUE(key_verify == key);

    // create - duplicate
    ASSERT_FALSE(db.insertKey(key)); 
}
