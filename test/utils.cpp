#include <unistd.h>

#include <cstdlib>
#include <errno.h>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

#include "utils.h"
#include "../common/utils.h"
#include "../grpc/include/grpc/impl/codegen/fork.h"

int spawn(const std::string& program, const std::vector<std::string>& args)
{
    pid_t child_pid;
    //grpc_prefork();
    child_pid = fork();
    if(child_pid == 0)
    {
        //grpc_postfork_child();
        std::vector<char*> argc;
        for(const auto& arg : args)
            argc.push_back(const_cast<char*>(arg.c_str()));
        argc.push_back(NULL);
        if(!argc.empty())
        {
            std::cout << "Forking " << argc[0] << " with args..." << std::endl;
            for(const auto& arg : argc)
                std::cout << "\t" << arg << std::endl;
        }
        execvp(program.c_str(), argc.data());
    }
    else
    {
        //grpc_postfork_parent();
        sleep(2); // HACK: Wait for the server to start
    }
    return child_pid;
}

int startServer(const std::string& serverName)
{
    std::stringstream command;
    command << getDir() << serverName;
    std::vector<std::string> args;
    args.push_back(serverName);
    return spawn(command.str(), args);
}

int startServer(const std::string& serverName, const std::vector<std::string>& args)
{
    std::stringstream command;
    command << getDir() << serverName;
    return spawn(command.str(), args);
}

void killServer(int pid)
{
    kill(pid, SIGTERM);
    int status = 0;
    do
    {
        waitpid(pid, &status, 0);
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
}
