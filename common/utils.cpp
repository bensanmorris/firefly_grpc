#include "utils.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <dlfcn.h>

#include <fstream>
#include <streambuf>
#include <string>

using namespace std;

static const int MAX_HOST_NAME_LEN = 1024;

void read(const std::string& file, std::string& out)
{
    std::ifstream f(file);
    if(!f.is_open())
        return;
    f.seekg(0, std::ios::end);
    out.reserve(f.tellg());
    f.seekg(0, std::ios::beg);
    out.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
}

std::string getHostName()
{
    struct addrinfo hints, *info;

    char hostname[MAX_HOST_NAME_LEN];
    hostname[0] = '\0';
    gethostname(hostname, MAX_HOST_NAME_LEN - 1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    int result = getaddrinfo(hostname, NULL, &hints, &info);
    if(result != 0)
    {
        freeaddrinfo(info);
        return "";
    }

    std::string name = info->ai_canonname;
    freeaddrinfo(info);

    return name;
}

std::string getDir()
{
    static std::string path;
    if(!path.empty())
        return path;
    
    Dl_info dl_info;
    dladdr((void *)getDir, &dl_info);
    path = dl_info.dli_fname;
    int dirEnd = path.find_last_of('/');
    path = path.substr(0, dirEnd);
    path += '/';
    return path;
}
