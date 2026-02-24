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
        // Каждый класс, наследующийся от миксина, получает два готовых объекта-события
        Event<PropertyChangedEventArgs> property_changed;
        Event<PropertyChangingEventArgs> property_changing;

    protected: 
        // Шаблонный метод для безопасного изменения свойства
        template<typename T>
        void set_property(const std::string& name, T& field, const T& new_value) // field - ссылка на само поле класса, которое мы меняем
        {
            // Если мы еще в конструкторе - просто присваиваем значение
            if (!events_enabled_) 
            {
                field = new_value;
                return;
            }

            std::any old_val = field; // Упаковываем старое и новое значения в контейнер std::any
            std::any new_val = new_value; // Это нужно, чтобы передать их в аргументы события универсально, не зная точного типа T

            // Создаем обьект аргументов для события "до изменения" (по умолчанию can_change = true)
            PropertyChangingEventArgs args_before(name, old_val, new_val);
            // Вызов события - в этот момент срабатывают все валидаторы
            property_changing(this, args_before); // this - указатель на текущий обьект (Frog/Toad)

            // Если какой-то обработчик установил can_change = false
            if (!args_before.can_change) 
                return;

            // Если валидация пройдена, применяем новое значение к полю
            field = new_value;

            // Создание аргументов для события "после изменения"
            PropertyChangedEventArgs args_after(name);
            // Вызов события - оповещаем всех подписчиков, что изменение успешно произошло
            property_changed(this, args_after);
        }

        // Включает события после завершения конструктора
        void enable_events() 
        {
            events_enabled_ = true;
        }
};