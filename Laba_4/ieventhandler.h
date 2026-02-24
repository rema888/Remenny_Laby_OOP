#pragma once

// Шаблонный интерфейс обработчика события
template<typename TEventArgs>
class IEventHandler 
{
    public:
        // 1й аргумент — указатель на объект, который вызвал событие
        // 2й аргумент — данные события
        virtual void handle(const void* sender, TEventArgs& args) = 0;

        virtual ~IEventHandler() = default;
}; 