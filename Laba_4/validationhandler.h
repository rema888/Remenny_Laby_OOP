#pragma once
#include "ieventhandler.h"
#include "propertychangingeventargs.h"
#include <string>

// Валидатор, который проверяет данные перед изменением свойства и может заблокировать это изменение
class ValidationHandler : public IEventHandler<PropertyChangingEventArgs> 
{
    public:
    
        void handle(const void* /*sender*/, PropertyChangingEventArgs& args) override 
        {
            // Запрещаем изменение, если новое значение — пустая строка
            if (args.new_value.type() == typeid(std::string)) 
            {
                const std::string& new_str = std::any_cast<const std::string&>(args.new_value);

                if (new_str.empty()) 
                {
                    std::cout << "[Валидатор] Отмена: Пустая строка недопустима.\n";
                    args.can_change = false;
                    return;
                }
            }

            // Запрещаем изменение свойств, начинающихся с '_' (например, приватные поля)
            if (!args.property_name.empty() && args.property_name[0] == '_') 
            {
                std::cout << "[Валидатор] Отмена: Нельзя менять свойства, начинающиеся с '_'.\n";
                args.can_change = false; 
                return;
            } 

            // Во всех остальных случаях — разрешаем
            args.can_change = true; 
        }
};