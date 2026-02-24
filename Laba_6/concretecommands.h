#pragma once

#include "resourcecontainer.h"
#include "icommand.h"
#include <iostream>
#include <string>

class KeyCommand : public ICommand 
{
    private:

        char keyChar;
        std::string& outputBuffer; 
        bool executed;

    public:

        KeyCommand(std::string& buffer, char k) 
            : outputBuffer(buffer), keyChar(k), executed(false) {}

        // Конструктор для фабрики (принимает контейнер)
        KeyCommand(ResourceContainer& res) 
            : outputBuffer(res.get<std::string>("buffer")), keyChar(0), executed(false) {}

        void execute() override 
        {
            // Добавляем символ в конец строки
            outputBuffer += keyChar;
            executed = true;
        }

        void undo() override 
        {
            // Выполнена ли команда и не пуст ли буфер
            if (executed && !outputBuffer.empty()) 
            {
                // Удаляем последний символ 
                outputBuffer.pop_back();
                executed = false;
            }
        }

        std::string getDescription() const override 
        {
            return std::string("Printed: '") + keyChar + "'";
        }

        // Хелпер для сериализации: вернуть тип команды
        std::string getTypeName() const override 
        {
            return "KeyCommand";
        }
        
        // Хелпер для сериализации: вернуть символ
        char getKey() const 
        { 
            return keyChar; 
        }

        bool isTextOutputCommand() const override 
        {
            return true;
        }

        // Сериализация 

        // Превращаем параметры в словарь
        ParamDict toDict() const override 
        {
            ParamDict d;
            if (keyChar != 0) 
                // создаем в словаре запись с ключом key
                d["key"] = std::string(1, keyChar);
            return d;
        }

        // Восстанавливаем параметры из словаря
        void fromDict(const ParamDict& data) override 
        {
            auto it = data.find("key");
            if (it != data.end() && !it->second.empty()) 
            {
                keyChar = it->second[0];
            } 
            else 
            {
                keyChar = 0;
            }
        }

        // Строитель для реестра (принимает только контейнер)
        static std::shared_ptr<ICommand> build(ResourceContainer& res) 
        {
            return std::make_shared<KeyCommand>(res);
        }
    };

// Регистрация
static bool regKey = (ICommand::registerType("KeyCommand", KeyCommand::build), true);

class VolumeUpCommand : public ICommand 
{
    private:
        // Cсылка на переменную громкости
        int& volumeLevel;
        // Шаг изменения
        int step;
        bool executed;

    public:

        VolumeUpCommand(int& vol, int s = 20) 
            : volumeLevel(vol), step(s), executed(false) {}

        // Конструктор для фабрики (принимает контейнер)
        VolumeUpCommand(ResourceContainer& res) 
            : volumeLevel(res.get<int>("volume")), step(20), executed(false) {}

        void execute() override 
        {
            volumeLevel += step;
            executed = true;
        }

        void undo() override 
        {
            if (executed) 
            {
                volumeLevel -= step;
                executed = false;
            }
        }

        std::string getDescription() const override 
        {
            return "Volume increased +" + std::to_string(step) + "% (Current: " + std::to_string(volumeLevel) + ")";
        }

        std::string getTypeName() const override 
        {
            return "VolumeUpCommand";
        }

        // Сериализация

        ParamDict toDict() const override 
        {
            ParamDict d;
            d["step"] = std::to_string(step);
            return d;
        }

        void fromDict(const ParamDict& data) override 
        {
            auto it = data.find("step");
            if (it != data.end()) 
            {
                try 
                {
                    step = std::stoi(it->second);
                }
                catch (...) 
                {
                    step = 20; // Защита от ошибок
                }
            }
        }

        // Строитель для реестра
        static std::shared_ptr<ICommand> build(ResourceContainer& res) 
        {
            return std::make_shared<VolumeUpCommand>(res);
        }
};

static bool regVolUp = (ICommand::registerType("VolumeUpCommand", VolumeUpCommand::build), true);

class VolumeDownCommand : public ICommand 
{
    private:

        int& volumeLevel;
        int step;
        bool executed;

    public:

        VolumeDownCommand(int& vol, int s = 20) 
            : volumeLevel(vol), step(s), executed(false) {}

        VolumeDownCommand(ResourceContainer& res) 
            : volumeLevel(res.get<int>("volume")), step(20), executed(false) {}

        void execute() override 
        {
            volumeLevel -= step;
            if (volumeLevel < 0) volumeLevel = 0;
            executed = true;
        }

        void undo() override 
        {
            if (executed) 
            {
                volumeLevel += step;
                executed = false;
            }
        }

        std::string getDescription() const override 
        {
            return "Volume decreased -" + std::to_string(step) + "% (Current: " + std::to_string(volumeLevel) + ")";
        }

        std::string getTypeName() const override 
        {
            return "VolumeDownCommand";
        }

        // Сериализация

        ParamDict toDict() const override 
        {
            ParamDict d;
            d["step"] = std::to_string(step);
            return d;
        }

        void fromDict(const ParamDict& data) override 
        {
            auto it = data.find("step");
            if (it != data.end()) 
            {
                try 
                {
                    step = std::stoi(it->second);
                } 
                catch (...) 
                {
                    step = 20;
                }
            }
        }

        // Строитель для реестра
        static std::shared_ptr<ICommand> build(ResourceContainer& res) 
        {
            return std::make_shared<VolumeDownCommand>(res);
        }
};
static bool regVolDown = (ICommand::registerType("VolumeDownCommand", VolumeDownCommand::build), true);

class MediaPlayerCommand : public ICommand 
{
    private:
        // Ссылка на флаг состояния плеера
        bool& isRunning;
        // Переменная для запоминания состояния до включения
        bool previousState;
        bool executed;

    public:
    
        MediaPlayerCommand(bool& running) 
            : isRunning(running), previousState(false), executed(false) {}

        // Конструктор для фабрики через контейнер
        MediaPlayerCommand(ResourceContainer& res)
            : isRunning(res.get<bool>("player")), previousState(false), executed(false) {}

        void execute() override 
        {
            // Запоминаем текущее состояние
            previousState = isRunning;
            isRunning = true;
            executed = true;
        }

        void undo() override 
        {
            if (executed) 
            {
                // Возвращаем то состояние, которое было до execute()
                isRunning = previousState;
                executed = false;
            }
        }

        std::string getDescription() const override 
        {
            return isRunning ? "Media Player Launched" : "Media Player Closed";
        }

        std::string getTypeName() const override 
        {
            return "MediaPlayerCommand";
        }

        // Сериализация

        // Этой команде нечего сохранять, кроме состояния, которое восстанавливается само
        ParamDict toDict() const override
        {
            return {}; // Пустой словарь
        }

        void fromDict(const ParamDict&) override 
        {
            // Нечего восстанавливать
        }

        // Строитель для реестра
        static std::shared_ptr<ICommand> build(ResourceContainer& res) 
        {
            return std::make_shared<MediaPlayerCommand>(res);
        }
};
static bool regPlayer = (ICommand::registerType("MediaPlayerCommand", MediaPlayerCommand::build), true);

class BrightnessCommand : public ICommand 
{
    private:
        int& brightnessLevel; // Ссылка на состояние в Keyboard
        int step;             // Уникальный параметр команды
        bool executed;
        int previousValue;    // Для отмены действия 

    public:
 
        BrightnessCommand(int& bright, int s = 10) 
            : brightnessLevel(bright), step(s), executed(false), previousValue(0) {}

        // Конструктор для фабрики (через контейнер ресурсов)
        BrightnessCommand(ResourceContainer& res) 
            : brightnessLevel(res.get<int>("brightness")), step(10), executed(false), previousValue(0) {}

        void execute() override 
        {
            previousValue = brightnessLevel;
            brightnessLevel += step;
            if (brightnessLevel > 100) brightnessLevel = 100;
            if (brightnessLevel < 0) brightnessLevel = 0;
            executed = true;
        }

        void undo() override 
        {
            if (executed) 
            {
                brightnessLevel = previousValue;
                executed = false;
            }
        }

        std::string getDescription() const override 
        {
            return "Brightness changed by " + std::to_string(step) + 
                   "% (Current: " + std::to_string(brightnessLevel) + "%)";
        }

        std::string getTypeName() const override 
        { 
            return "BrightnessCommand"; 
        }

        // Сериализация

        ParamDict toDict() const override 
        {
            ParamDict d;
            d["step"] = std::to_string(step);
            return d;
        }

        void fromDict(const ParamDict& data) override 
        {
            auto it = data.find("step");
            if (it != data.end()) 
            {
                try {
                    step = std::stoi(it->second);
                } catch (...) {
                    step = 10; // Защита от ошибок
                }
            }
        }

        // Строитель для реестра (берет ресурс по имени "brightness")
        static std::shared_ptr<ICommand> build(ResourceContainer& res) 
        {
            return std::make_shared<BrightnessCommand>(res);
        }
};

// Автоматическая регистрация в реестре
static bool regBright = (ICommand::registerType("BrightnessCommand", BrightnessCommand::build), true);