#pragma once

#include "resourcecontainer.h"
#include <string>
#include <memory>
#include <iostream>
#include <map>
#include <functional>

// Обьявляем тип словаря параметров
using ParamDict = std::map<std::string, std::string>;

// Строитель принимает только контейнер ресурсов
// Сколько бы ресурсов ни было добавлено, сигнатура не меняется
using CommandBuilder = std::function<std::shared_ptr<class ICommand>(ResourceContainer&)>;

class ICommand 
{
    public:
        // Метод выполнения действия команды
        virtual void execute() = 0;

        // Метод отмены действия команды
        virtual void undo() = 0;

        // Метод получения строкового представления действия
        virtual std::string getDescription() const = 0;

         // Метод для сериализации
        virtual std::string getTypeName() const = 0; 

        virtual ~ICommand() = default;

        // Метод для отличия текстовых команд от остальных
        virtual bool isTextOutputCommand() const 
        { 
            return false; 
        }

        // Самосериализация
        virtual ParamDict toDict() const = 0; // Превращает внутренние параметры команды в словарь строк

        virtual void fromDict(const ParamDict& data) = 0; // Восстанавливает внутренние параметры команды из словаря

        // Фабрика и регистрация
        static void registerType(const std::string& name, CommandBuilder builder);

        static std::shared_ptr<ICommand> createInstance(
            const std::string& type, 
            ResourceContainer& resources // Один аргумент навсегда
        );
};

// Псевдоним типа для умного указателя на команду
using CommandPtr = std::shared_ptr<ICommand>;

// Реализация фабрики

inline std::map<std::string, CommandBuilder>& getRegistry() 
{
    static std::map<std::string, CommandBuilder> registry;
    return registry;
}

inline void ICommand::registerType(const std::string& name, CommandBuilder builder) 
{
    getRegistry()[name] = builder;
}

inline std::shared_ptr<ICommand> ICommand::createInstance(const std::string& type, ResourceContainer& resources) 
{
    auto& reg = getRegistry(); // Получаем реестр
    auto it = reg.find(type); // Ищем в справочнике запись с именем, которое пришло из файла (например, BrightnessCommand)
    if (it != reg.end())
        return it->second(resources); // Передаем контейнер
    // Вызывается код конкретной команды (например, BrightnessCommand::build), который лезет в контейнер, берет нужные данные ("brightness") 
    // и создает объект new BrightnessCommand(...). Фабрика возвращает готовый объект

    std::cerr << "[Factory] Unknown command type: " << type << std::endl;
    return nullptr;
}