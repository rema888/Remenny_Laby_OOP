#pragma once 

#include "loglevel.h"
#include <string>

class ILogFilter 
{
    public:
    
        virtual bool match(LogLevel level, const std::string& text) const = 0;

        virtual ~ILogFilter() = default;
};