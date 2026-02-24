#pragma once
#include "propertynotifiermixin.h"
#include <string>

class Frog : public PropertyNotifierMixin 
{
    private:

        std::string name_;
        std::string color_;
        double weight_;

    public:

        Frog(const std::string& name, const std::string& color, double weight)
            : name_(name), color_(color), weight_(weight) 
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

        // Свойство color
        const std::string& get_color() const 
        { 
            return color_; 
        }

        void set_color(const std::string& value) 
        {
            set_property("color", color_, value);
        }

        // Свойство weight
        double get_weight() const 
        { 
            return weight_; 
        }
        
        void set_weight(double value) 
        {
            set_property("weight", weight_, value);
        }
};