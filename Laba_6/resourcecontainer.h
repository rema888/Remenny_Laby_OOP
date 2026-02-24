#pragma once

#include <string>
#include <map>
#include <any> 
#include <stdexcept> 

class ResourceContainer 
{
    private:

        std::map<std::string, std::any> resources;

    public:

        // Положить ресурс
        template<typename T>
        void set(const std::string& name, T& value) 
        {
            resources[name] = &value; // Храним указатель, чтобы менять оригинал
        }

        // Получить ресурс по имени
        template<typename T>
        T& get(const std::string& name) 
        {
            auto it = resources.find(name);
            if (it == resources.end())   
                throw std::runtime_error("Resource not found: " + name);  
            // Извлекаем указатель и приводим к нужному типу
            return *std::any_cast<T*>(it->second);
        }
};