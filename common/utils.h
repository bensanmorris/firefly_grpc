#pragma once

#include <memory>
#include <string>

void read(const std::string& file, std::string& out);
std::string getHostName();
std::string getDir();

namespace std
{
    template<typename T, typename ...Args>
    std::unique_ptr<T> make_unique( Args&& ...args ) 
    {
        return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
    }
}
