#pragma once 

#include "loglevel.h"
#include <string>
 
class ILogHandler 
{
    public:

        virtual void handle(LogLevel level, const std::string& text) const = 0;

        virtual ~ILogHandler() = default;
};