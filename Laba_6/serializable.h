#pragma once

#include <string>
#include <vector>
#include <map>

// Имя поля -> Значение поля, приведенное к строке 
using DataDict = std::map<std::string, std::string>;

class ISerializable 
{
    public:
        // Превратить объект в словарь
        virtual DataDict toDict() const = 0;

        // Восстановить объект из словаря
        virtual void fromDict(const DataDict& data) = 0;

        virtual ~ISerializable() = default;
};