#pragma once
#include "event.h"
#include "propertychangedeventargs.h"
#include "propertychangingeventargs.h"
#include <any>

class PropertyNotifierMixin 
{
    private:

        bool events_enabled_ = false; // Защита от вызова событий в конструкторе

    public:

        Event<PropertyChangedEventArgs> property_changed;
        Event<PropertyChangingEventArgs> property_changing;

    protected:
        // Шаблонный метод для безопасного изменения свойства
        template<typename T>
        void set_property(const std::string& name, T& field, const T& new_value) 
        {
            if (!events_enabled_) 
            {
                field = new_value;
                return;
            }

            std::any old_val = field;
            std::any new_val = new_value;

            PropertyChangingEventArgs args_before(name, old_val, new_val);
            property_changing(this, args_before);

            if (!args_before.can_change) 
                return;

            field = new_value;

            PropertyChangedEventArgs args_after(name);
            property_changed(this, args_after);
        }

        // Включает события после завершения конструктора
        void enable_events() 
        {
            events_enabled_ = true;
        }
};