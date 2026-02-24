#pragma once
#include "eventargs.h"  
#include <string>
#include <any>

// Аргументы события: свойство собирается измениться
class PropertyChangingEventArgs : public EventArgs 
{
    public: 
    
        std::string property_name;
        std::any old_value;
        std::any new_value;
        bool can_change;

        PropertyChangingEventArgs(const std::string& name, const std::any& old_val,
            const std::any& new_val, bool can_change = true): 
            property_name(name), old_value(old_val), 
            new_value(new_val), can_change(can_change)
        {}
};