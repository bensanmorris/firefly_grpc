#include "server.h"

using namespace ffsrv;

int main(int argc, char** argv)
{
    ServerService server;
    return server.start(argc, argv);
}

