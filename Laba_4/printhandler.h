#pragma once
#include "ieventhandler.h"
#include "propertychangedeventargs.h"
#include <iostream>

class PrintHandler : public IEventHandler<PropertyChangedEventArgs> 
{
    public:

        void handle(const void* /*sender*/, PropertyChangedEventArgs& args) override 
        {
            std::cout << "Свойство изменилось: " << args.property_name << "\n";
        }
};