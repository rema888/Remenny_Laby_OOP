#pragma once

#include "ieventhandler.h"
#include <vector>
#include <algorithm> 

template<typename TEventArgs>
class Event 
{
    private:
        // Вектор указателей на обработчики
        // Позволяет хранить в одном списке разные реализации обработчиков (PrintHandler и ValidationHandler)
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
                // erase - удаляет всё от начала мусора до конца
                handlers_.erase(
                    // remove - все элементы, которые не будем удалять, смещаются влево, а в конце остаются данные для удаления(мусор)
                    std::remove(handlers_.begin(), handlers_.end(), handler),
                    handlers_.end());
        }
       
        // Оповещение всех подписчиков
        void invoke(const void* sender, TEventArgs& args) 
        {
            for (auto* handler : handlers_) 
            {
                if (handler != nullptr) 
                    handler->handle(sender, args);
            }
        }

        // Позволяет вызывать событие как функцию
        // Вместо event.invoke(obj, args) можно написать event(obj, args)
        void operator()(const void* sender, TEventArgs& args) 
        {
            invoke(sender, args);
        }
};