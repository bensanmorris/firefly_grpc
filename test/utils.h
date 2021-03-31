#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "../common/utils.h"

int startServer(const std::string&);
int startServer(const std::string&, const std::vector<std::string>& args);
void killServer(int);

struct ServerGuard
{
    ServerGuard(const std::string& serverName)
    {
        pid = startServer(serverName);
    }
    ServerGuard(const std::string& serverName, const std::map<std::string, std::string>& argsList)
    {
        std::vector<std::string> args;
        args.push_back(serverName);
        for(auto item : argsList)
        {
            args.push_back(item.first);
            args.push_back(item.second);
        }
        pid = startServer(serverName, args);
    }
    ~ServerGuard()
    {
        kill();
    }
    void kill()
    {
        killServer(pid);
    }
    int pid;
};
