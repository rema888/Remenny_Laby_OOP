#pragma once

#include "ilogfilter.h"
#include <string>
#include <regex> 
#include <string>

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
    
        std::string pattern_;

    public:

        explicit SimpleLogFilter(const std::string& pattern) : pattern_(pattern) {}

        bool match(LogLevel /*level*/, const std::string& text) const override 
        {
            return text.find(pattern_) != std::string::npos;
        }
};
 
// Фильтр по регулярному выражению 
class ReLogFilter : public ILogFilter 
{
    private:

        std::regex pattern_;
        bool valid_ = true;  

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

        bool match(LogLevel /*level*/, const std::string& text) const override 
        {
            if (!valid_) return false;
            return std::regex_search(text, pattern_);
        }
};