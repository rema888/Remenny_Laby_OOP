#pragma once

#include "ilogformatter.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
 
class SimpleFormatter : public ILogFormatter 
{
    public:

        std::string format(LogLevel level, const std:: string& text) const override 
        {
            std::string level_str;
            switch (level) 
            {
                case LogLevel::INFO:  level_str = "INFO";  break;
                case LogLevel::WARN:  level_str = "WARN";  break;
                case LogLevel::ERROR: level_str = "ERROR"; break;
            }

            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            std::ostringstream oss;
            oss << std::put_time(std::localtime(&time_t), "%Y.%m.%d %H:%M:%S");

            std::string time_str = oss.str();

            return "[" + level_str + "] [" + time_str + "] " + text;
        }
};