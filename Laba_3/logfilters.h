#pragma once

#include "ilogfilter.h"
#include <string>
#include <regex> 


// Фильтр по уровню лога
class LevelFilter : public ILogFilter 
{
    private:

        LogLevel required_level_;

    public:

        explicit LevelFilter(LogLevel level) : required_level_(level) {}

        bool match(LogLevel level, const std::string& /*text*/) const override 
        {
            return level == required_level_;
        }
};

// Фильтр по наличию подстроки в тексте
class SimpleLogFilter : public ILogFilter 
{
    private:
    
        std::string substr_; 

    public:

        explicit SimpleLogFilter(const std::string& substr) : substr_(substr) {}

        bool match(LogLevel /*level*/, const std::string& text) const override 
        {
            return text.find(substr_) != std::string::npos;
        }
};
   
// Фильтр по регулярному выражению 
class ReLogFilter : public ILogFilter 
{
    private:

        std::regex pattern_;
        bool valid_ = true; // успешно ли было создано регулярное выражение

    public:

        explicit ReLogFilter(const std::string& pattern) 
        {
            try 
            {
                pattern_ = std::regex(pattern);
            } 
            catch (const std::regex_error&) 
            {
                valid_ = false;  
            }
        }

        bool match(LogLevel /*level*/, const std::string& text) const 
        {
            if (!valid_) 
                return false;
            return std::regex_search(text, pattern_);
        }
};