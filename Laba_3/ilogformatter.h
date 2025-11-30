#pragma once 

#include "loglevel.h"
#include <string>
  
class ILogFormatter 
{
    public:

        virtual std::string format(LogLevel level, const std::string& text) const = 0;

        virtual ~ILogFormatter() = default;
};