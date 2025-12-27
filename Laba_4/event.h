#pragma once

#include "ieventhandler.h"
#include <vector>
#include <algorithm>

template<typename TEventArgs>
class Event 
{
    private:

        std::vector<IEventHandler<TEventArgs>*> handlers_;

    public:
        
        // Подписка
        void operator+=(IEventHandler<TEventArgs>* handler) 
        {
            if (handler != nullptr)
                handlers_.push_back(handler);    
        }

        // Отписка
        void operator-=(IEventHandler<TEventArgs>* handler) 
        {
            if (handler != nullptr) 
                handlers_.erase(
                    std::remove(handlers_.begin(), handlers_.end(), handler),
                    handlers_.end());
        }
       
        // Оповещение всех подписчиков
        void invoke(const void* sender, const TEventArgs& args) 
        {
            for (auto* handler : handlers_) 
            {
                if (handler != nullptr) 
                    handler->handle(sender, args);
            }
        }

        // Позволяет вызывать событие как функцию
        void operator()(const void* sender, const TEventArgs& args) 
        {
            invoke(sender, args);
        }
};