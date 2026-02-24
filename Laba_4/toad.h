#pragma once
#include "propertynotifiermixin.h"
#include <string>

class Toad : public PropertyNotifierMixin 
{
    private:

        std::string name_;
        std::string _address_;
        int age_;

    public:

        Toad(const std::string& name, const std::string& address, int age)
            : name_(name), _address_(address), age_(age) 
        {
            enable_events(); // Включаем события после инициализации
        }

        // Свойство name
        const std::string& get_name() const 
        { 
            return name_; 
        }

        void set_name(const std::string& value) 
        {
            set_property("name", name_, value);
        }

        // Свойство _address
        const std::string& get__address() const 
        { 
            return _address_; 
        }

        void set__address(const std::string& value) 
        {
            set_property("_address", _address_, value); // Имя начинается с _
        }

        // Свойство age
        int get_age() const 
        { 
            return age_; 
        }
        
        void set_age(int value) 
        {
            set_property("age", age_, value);
        }
};