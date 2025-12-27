#pragma once
#include "eventargs.h"  
#include <string>

// Аргументы события: свойство изменилось
struct PropertyChangedEventArgs : public EventArgs 
{
    std::string property_name;

    explicit PropertyChangedEventArgs(const std::string& name): property_name(name) {}
};