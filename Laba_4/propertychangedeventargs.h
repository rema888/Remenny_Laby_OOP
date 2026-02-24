#pragma once
#include "eventargs.h"  
#include <string>

// Аргументы события: свойство изменилось
class PropertyChangedEventArgs : public EventArgs 
{
    public: 

        std::string property_name; // Имя свойства, которое было изменено

        explicit PropertyChangedEventArgs(const std::string& name): property_name(name) {}
};