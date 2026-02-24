#pragma once

#include "resourcecontainer.h"
#include "concretecommands.h"
#include "keyboard.h"
#include "jsonserializer.h"
#include <memory>

class KeyboardStateSaver 
{
    private:

        std::string filename; // Имя файла конфигурации

        // Фабрика теперь принимает только тип и контейнер ресурсов.
        // Никаких отдельных ссылок на buffer, volume и т.д.
        CommandPtr createCommandFromType(const std::string& type, ResourceContainer& res) 
        {
            return ICommand::createInstance(type, res);
        }

    public:

        explicit KeyboardStateSaver(const std::string& file) : filename(file) {}

        // Сохранение состояния. Используем метод toDict() у каждой команды
        void save(Keyboard& keyboard) 
        {
            DataDict data; // Создаем пустой словарь
            
            // Получаем карту всех привязок из клавиатуры
            const auto& commands = keyboard.getCommandsMap();
            int index = 0; // Счетчик для создания уникальных ключей
            
            // Проходим по каждой привязке (клавиша -> команда)
            for (const auto& pair : commands) 
            {
                const std::string& keyName = pair.first;  // Имя клавиши
                const CommandPtr& cmd = pair.second;      // Умный указатель на команду

                // Так как JSON у нас плоский (один уровень), создаем уникальные префиксы:
                // key_0_, key_1_, key_2_ и т.д.
                std::string prefix = "key_" + std::to_string(index) + "_";
                
                // 1. Сохраняем имя клавиши и тип
                data[prefix + "name"] = keyName;
                data[prefix + "type"] = cmd->getTypeName();

//////          // KeyboardStateSaver не знает про step, level или другие поля
                // Он просто просит команду саму отдать её данные
                // Команда возвращает словарь (например, {"step": "10"} или {"key": "a"}).
                ParamDict params = cmd->toDict();
                
                // 3. Превращаем этот словарь в JSON-строку и кладем в общий файл
                // Это делает хранение универсальным: какие бы поля ни вернула команда, мы просто сохраняем их как строку
                if (!params.empty()) 
                {
                    data[prefix + "params"] = JsonSerializer::serialize(params);
                } 
                else 
                {
                    data[prefix + "params"] = ""; // Пусто, если параметров нет
                }

                index++;
            }
            
            // 4. Сохраняем общее количество команд, чтобы при загрузке знать, сколько циклов делать
            data["total_keys"] = std::to_string(index);

            // 5. Отдаем словарь сериализатору для записи в файл
            JsonSerializer::saveToFile(data, filename);
        }

        // Загрузка состояния и восстановление ассоциаций. Создаем объект, затем вызываем fromDict()     
        void load(Keyboard& keyboard) 
        {
            // 1. Читаем файл и превращаем его в словарь
            DataDict data = JsonSerializer::loadFromFile(filename);
            
            // Проверка: если файл пуст или не открылся, выходим
            if (data.empty()) 
            {
                std::cerr << "[Loader] No data loaded from " << filename << std::endl;
                return;
            }

            // 2. Узнаем, сколько команд было сохранено
            int total = 0;
            auto itTotal = data.find("total_keys");
            // Безопасное преобразование строки в число
            if (itTotal != data.end() && !itTotal->second.empty()) 
            {
                try 
                {
                    // Ищем число сохраненных команд
                    total = std::stoi(itTotal->second);
                } 
                catch (...) 
                {
                    std::cerr << "[Loader] Error parsing total_keys." << std::endl;
                    return;
                }
            }
            else 
            {
                std::cerr << "[Loader] 'total_keys' not found." << std::endl;
                return
                ;
            }
            // Создаем контейнер ресурсов и наполняем его всеми доступными данными клавиатуры
            // Если добавится яркость, мы просто добавим одну строчку .set() сюда
            // Код фабрики и команд менять не нужно
            ResourceContainer res;
            res.set("buffer", keyboard.getPrintedSeqRef());
            res.set("volume", keyboard.getVolumeLevelRef());
            res.set("player", keyboard.isPlayerRunningRef());
            res.set("brightness", keyboard.getBrightnessLevelRef());
            // 3. Цикл восстановления привязок
            for (int i = 0; i < total; i++) 
            {
                std::string prefix = "key_" + std::to_string(i) + "_";
                
                // Ищем в словаре данные для текущего индекса
                auto itName = data.find(prefix + "name");
                auto itType = data.find(prefix + "type");
                auto itParams = data.find(prefix + "params"); // Читаем строку с параметрами

                // Если данных нет (битая запись), пропускаем эту итерацию
                if (itName == data.end() || itType == data.end()) 
                    continue; 

                std::string keyName = itName->second; // Имя клавиши 
                std::string type = itType->second;    // Тип команды
                
                // Команда сама вытащит оттуда только то, что ей нужно (buffer или volume или brightness).
                CommandPtr newCmd = createCommandFromType(type, res);
                
                if (newCmd) 
                {
                    // 4. Восстанавливаем внутренние параметры команды (step, keyChar) из файла
                    if (itParams != data.end() && !itParams->second.empty()) {
                        ParamDict params = JsonSerializer::deserialize(itParams->second);
                        newCmd->fromDict(params);
                    }
                    
                    keyboard.bindKey(keyName, newCmd);
                    std::cout << "[Loader] Restored binding: " << keyName << " -> " << type << std::endl;
                }
            }
        }
};