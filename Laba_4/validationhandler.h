#pragma once
#include "ieventhandler.h"
#include "propertychangingeventargs.h"
#include <string>

class ValidationHandler : public IEventHandler<PropertyChangingEventArgs> 
{
    public:
        void handle(const void* sender, const PropertyChangingEventArgs& args) override 
        {
            // Запрещаем изменение, если новое значение — пустая строка
            if (args.new_value.type() == typeid(std::string)) 
            {
                std::string new_str = std::any_cast<std::string>(args.new_value);

                if (new_str.empty()) 
                {
                    const_cast<PropertyChangingEventArgs&>(args).can_change = false;
                    return;
                }
            }

            // Запрещаем изменение свойств, начинающихся с '_'
            if (!args.property_name.empty() && args.property_name[0] == '_') 
            {
                const_cast<PropertyChangingEventArgs&>(args).can_change = false;
                return;
            } 

            // Во всех остальных случаях — разрешаем
            const_cast<PropertyChangingEventArgs&>(args).can_change = true;
        }
};