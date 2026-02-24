#pragma once

#include "icommand.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <memory>

class Keyboard 
{
    private:
        // Буфер напечатанного текста
        std::string printedSeq;
        int volumeLevel;
        int brightnessLevel;
        bool isPlayerRunning;

        // Имя клавиши и умный указатель на обьект команды
        std::map<std::string, CommandPtr> commandsMap;

        // Стек выполненных и отмененных действий
        std::vector<std::string> undoStack; // Кладем все, что сделали, чтобы можно было вернуть назад
        std::vector<std::string> redoStack; // Кладем то, что отменили, чтобы можно было вернуть вперед

        // Поток для логирования в файл
        std::ofstream logFile;

        // Вспомогательная функция для дублирования вывода в консоль и файл
        void logOutput(const std::string& message) 
        {
            // Вывод в консоль
            std::cout << message << std::endl;
            // Вывод в файл
            if (logFile.is_open()) 
            {
                logFile << message << std::endl;
                logFile.flush(); // Принудительный сброс буфера на диск
            }
        }

    public:

        explicit Keyboard(const std::string& logPath = "output.log") 
            : volumeLevel(50), isPlayerRunning(false) 
        {
            logFile.open(logPath);
            if (!logFile.is_open()) 
            {
                std::cerr << "Warning: Could not open log file " << logPath << ". Logging to file disabled." << std::endl;
            } 
            else 
            {
                logOutput("=== Keyboard Session Started ===");
            }
        }

        ~Keyboard() 
        {
            if (logFile.is_open()) 
            {
                logOutput("=== Keyboard Session Ended ===");
                logFile.close();
            }
        }

        // Привязка клавиши
        void bindKey(const std::string& key, CommandPtr command)
        {
            commandsMap[key] = command;
        }

        // Выполнение команды
        void pressKey(const std::string& key) 
        {
            auto it = commandsMap.find(key); // Ищем команду в словаре
            if (it == commandsMap.end()) 
            {
                logOutput("Unknown key: " + key); // Если нет такой клавиши
                return;
            }

            CommandPtr cmd = it->second;
            
            // Выполняем действие
            cmd->execute();

            // Формируем сообщение для файла в зависимости от типа команды
            std::string logMessage = "";
            
            // Если это команда печати - пишем текущую строку
            if (cmd->isTextOutputCommand()) 
            {
                logMessage = printedSeq;
            } 
            // Иначе берем описание действия (громкость, плеер)
            else 
            {
                logMessage = cmd->getDescription(); // Если добавим новую команду
            }

            // Пишем результат в файл
            if (logFile.is_open()) 
            {
                logFile << logMessage << std::endl;
                logFile.flush();
            }
            
            // Для наглядности в консоли выводим сам факт нажатия
            std::cout << key << std::endl; 

            undoStack.push_back(key); // Добавляем ключ в историю Undo
            redoStack.clear(); // Очищаем историю Redo
        }

        // Отмена (Undo)
        void undo() 
        {
            if (undoStack.empty()) 
            {
                std::cout << "undo" << std::endl; // В консоль показываем команду
                if (logFile.is_open()) 
                    logFile << "Nothing to undo." << std::endl; logFile.flush();        
                return;
            }

            std::string key = undoStack.back(); // Берем последний ключ
            undoStack.pop_back(); // Удаляем его из стека Undo

            auto it = commandsMap.find(key);
            if (it != commandsMap.end()) 
            {
                CommandPtr cmd = it->second;
                cmd->undo();

                std::cout << "undo" << std::endl; // В консоль команду

                // В файл пишем результат
                std::string logMessage = "";
                if (cmd->isTextOutputCommand()) 
                {
                    logMessage = printedSeq;
                } 
                else 
                {  
                   logMessage = cmd->getDescription(); 
                }
                
                if (logFile.is_open()) 
                {
                    logFile << logMessage << std::endl;
                    logFile.flush();
                }

                redoStack.push_back(key); // Кладем ключ в стек Redo
            }
        }

        // Повтор (Redo)
        void redo() 
        {
            if (redoStack.empty()) 
            {
                std::cout << "redo" << std::endl;
                if (logFile.is_open())              
                    logFile << "Nothing to redo." << std::endl; logFile.flush();                
                return;
            }

            std::string key = redoStack.back(); // Берем из стека Redo
            redoStack.pop_back();

            auto it = commandsMap.find(key);
            if (it != commandsMap.end()) 
            {
                CommandPtr cmd = it->second;
                cmd->execute();

                std::cout << "redo" << std::endl;

                std::string logMessage = "";
                if (cmd->isTextOutputCommand()) 
                {
                    logMessage = printedSeq;
                } 
                else 
                {
                    logMessage = cmd->getDescription();
                }

                if (logFile.is_open()) 
                {
                    logFile << logMessage << std::endl;
                    logFile.flush();
                }

                undoStack.push_back(key); // Возвращаем ключ в стек Undo
            }
        }

        // Геттеры для состояния  
        const std::string& getPrintedSeq() const
        { 
            return printedSeq; 
        }

        int getVolumeLevel() const 
        { 
            return volumeLevel; 
        }

        bool isPlayerOn() const 
        { 
            return isPlayerRunning; 
        }
        
        const std::map<std::string, CommandPtr>& getCommandsMap() const 
        { 
            return commandsMap; 
        }

        std::string& getPrintedSeqRef() 
        { 
            return printedSeq; 
        }

        int& getVolumeLevelRef() 
        { 
            return volumeLevel; 
        }

        int& getBrightnessLevelRef() 
        { 
            return brightnessLevel; 
        }
        bool& isPlayerRunningRef()
        { 
            return isPlayerRunning; 
        }
        
        // Очистка истории
        void clearHistory() 
        {
            undoStack.clear();
            redoStack.clear();
        }
};