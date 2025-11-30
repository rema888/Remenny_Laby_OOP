#pragma once

#include "ilogfilter.h"
#include "ilogformatter.h"
#include "iloghandler.h"
#include <vector>
#include <memory>

class Logger 
{
    private: 

        std::vector<std::unique_ptr<ILogFilter>> filters_;
        std::vector<std::unique_ptr<ILogFormatter>> formatters_;
        std::vector<std::unique_ptr<ILogHandler>> handlers_;

    public:

        Logger(
            std::vector<std::unique_ptr<ILogFilter>> filters,
            std::vector<std::unique_ptr<ILogFormatter>> formatters,
            std::vector<std::unique_ptr<ILogHandler>> handlers
        ) 
            : filters_(std::move(filters))
            , formatters_(std::move(formatters))
            , handlers_(std::move(handlers))
        {}
  
   
        void log(LogLevel level, const std::string& text) 
        {
            for (const auto& filter : filters_) 
            {
                if (!filter->match(level, text))           
                    return; // сообщение отклонено       
            }

      
            std::string formatted_text = text;
            for (const auto& formatter : formatters_) 
            {
                formatted_text = formatter->format(level, formatted_text);
            }

     
            for (const auto& handler : handlers_) 
            {
                handler->handle(level, formatted_text);
            }
        }

        void log_info(const std::string& text)  { log(LogLevel::INFO,  text); }
        void log_warn(const std::string& text)  { log(LogLevel::WARN,  text); }
        void log_error(const std::string& text) { log(LogLevel::ERROR, text); }
};