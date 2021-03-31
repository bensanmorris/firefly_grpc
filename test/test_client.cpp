#include <random>

#include "gtest/gtest.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "utils.h"
#include "../server/client.h"

using namespace ffsrv;

TEST(test_client, test_login)
{
    for(int i = 0; i < 1; ++i)
    {
        // client creation
        Client::InitialisationParameters client_args;
        client_args.ca   = "/keys/MyCA.pem";
        client_args.key  = "/keys/localhost-client-key.pem";
        client_args.cert = "/keys/localhost-client.pem";
        Client::Server server { getHostName(), 50051 };
        client_args.servers.push_back(server);
        Client client;
        ASSERT_TRUE(client.initialise(client_args));

        // client login
        Client::LoginParameters login_args;
        login_args.caller_identity = "alice";
        ASSERT_TRUE(client.login(login_args));

        // locations
        std::random_device rd;
        std::default_random_engine gen(rd());
        std::uniform_real_distribution<float> xdist(-30.f, 30.f);
        std::uniform_real_distribution<float> ydist(10.f, 50.f);
        std::uniform_real_distribution<float> zdist(0.f, -10.f);
        for(int j = 0; j < 100; j++)
        {
            ASSERT_TRUE(client.addLocation(xdist(gen),ydist(gen),zdist(gen),"some user data"));
            sleep(1);
        }

        // client logout
        ASSERT_TRUE(client.logout());

    }
}
