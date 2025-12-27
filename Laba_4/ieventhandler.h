#pragma once

// Шаблонный интерфейс обработчика события
template<typename TEventArgs>
class IEventHandler 
{
    public:

        virtual void handle(const void* sender, const TEventArgs& args) = 0;

        virtual ~IEventHandler() = default;
};