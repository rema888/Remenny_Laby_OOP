#pragma once

#include "serializable.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

class JsonSerializer 
{
    private:
        // Экранирование спецсимволов для JSON
        static std::string escapeJson(const std::string& s) 
        {
            std::string res;
            for (char c : s) // Проходим по каждому символу входной строки
            {
                if (c == '"') res += "\\\"";   // Кавычку заменяем на \"
                else if (c == '\\') res += "\\\\"; // Слэш заменяем на два слеша 
                else if (c == '\n') res += "\\n";  // Перенос строки заменяем на \n
                else res += c; // Остальные символы пишем как есть
            }
            return res;
        }

        // Обратное преобразование
        static std::string unescapeJson(const std::string& s) 
        {
            std::string res;
            res.reserve(s.length()); // Заранее резервируем память для скорости
            
            for (size_t i = 0; i < s.length(); i++) 
            {
                // Если видим слэш и есть следующий символ
                if (s[i] == '\\' && i + 1 < s.length()) 
                {
                    char next = s[i+1];
                    if (next == '"') res += '"';   // \" -> "
                    else if (next == '\\') res += '\\'; // \\ -> один слеш
                    else if (next == 'n') res += '\n';  // \n -> перенос строки
                    else res += s[i]; // Неизвестная комбинация, оставляем слэш
                    i++; // Пропускаем следующий символ, он уже обработан
                } 
                else 
                {
                    res += s[i]; // Обычный символ
                }
            }
            return res;
        }

    public:

        // Преобразует словарь (DataDict) в строку формата JSON
        static std::string serialize(const DataDict& data) 
        {
            std::ostringstream oss; // Поток для сборки строки
            oss << "{\n";           // Начало JSON объекта
            bool first = true;      // Флаг для обработки запятых
            
            for (const auto& pair : data) // Перебираем все пары ключ-значение из словаря
            {
                if (!first) oss << ",\n"; // Перед каждой парой (кроме первой) ставим запятую
                // Формируем строку: "ключ": "значение"
                // Ключ и значение оборачиваем в кавычки
                // Значение пропускаем через escapeJson, чтобы спецсимволы не сломали формат
                oss << "  \"" << pair.first << "\": \"" << escapeJson(pair.second) << "\"";
                first = false;
            }
            oss << "\n}"; // Конец JSON объекта
            return oss.str(); // Возвращаем готовую строку
        }

        // Преобразует строку формата JSON обратно в словарь 
        static DataDict deserialize(const std::string& jsonStr) 
        {
            DataDict result;
            
            // Защита от пустой входной строки
            if (jsonStr.empty()) return result;

            size_t pos = 0;              // Текущая позиция поиска в строке
            size_t len = jsonStr.length(); // Общая длина строки

            // Цикл проходит по всей строке, пока есть данные для обработки
            while (pos < len) 
            {
                // 1. Поиск и извлечение ключа    
                // Ищем открывающую кавычку ключа, начиная с текущей позиции
                size_t keyStart = jsonStr.find('"', pos);
                // Если кавычка не найдена или стоит в самом конце — выходим из цикла
                if (keyStart == std::string::npos || keyStart >= len - 1) break;

                // Ищем закрывающую кавычку ключа (начинаем поиск сразу после opening)
                size_t keyEnd = jsonStr.find('"', keyStart + 1);
                // Если закрывающей кавычки нет — файл поврежден, выходим
                if (keyEnd == std::string::npos || keyEnd >= len) break;

                // Вырезаем текст ключа между кавычками
                // substr(начало, длина) -> начало = keyStart + 1, длина = разница между концами минус 1
                std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);


                // 2. Поиск и извлечение значения 
                // Ищем открывающую кавычку значения (оно идет после ключа и двоеточия)
                size_t valStart = jsonStr.find('"', keyEnd + 1);
                if (valStart == std::string::npos || valStart >= len) break;

                // Ищем закрывающую кавычку значения
                size_t valEnd = jsonStr.find('"', valStart + 1);
                if (valEnd == std::string::npos || valEnd >= len) break;

                // Вырезаем сырое значение между кавычками
                std::string valueStr;
                if (valStart + 1 < valEnd) 
                {
                    // Если внутри есть символы, извлекаем их
                    valueStr = jsonStr.substr(valStart + 1, valEnd - valStart - 1);
                } 
                else 
                {
                    // Если кавычки стоят рядом "" — значение пустое
                    valueStr = "";
                }

                // 3. Распаковка спецсимволов           
                // Превращаем экранированные последовательности (\n, \", \\) 
                // обратно в реальные символы. Логика вынесена в отдельный метод.
                std::string value = unescapeJson(valueStr);

                // 4. Сохранение и подготовка к следующей итерации
                // Добавляем пару ключ-значение в результирующий словарь
                result[key] = value;
                
                // Сдвигаем позицию поиска сразу после конца текущего значения,
                // чтобы на следующем круге искать уже следующий ключ
                pos = valEnd + 1;
            }

            return result;
        }

         // Сохраняет словарь в файл с указанным именем.
        static void saveToFile(const DataDict& data, const std::string& filename) 
        {
            std::ofstream file(filename); // Открываем файл на запись
            if (file.is_open()) 
            {
                // Сначала конвертируем словарь в JSON-строку, затем пишем в файл
                file << serialize(data); 
                file.close(); // Закрываем файл, чтобы данные точно записались на диск
                std::cout << "[Serializer] State saved to " << filename << std::endl;
            } 
            else 
            {
                std::cerr << "[Serializer] Error saving to " << filename << std::endl;
            }
        }

        // Загружает словарь из файла с указанным именем.
        static DataDict loadFromFile(const std::string& filename) 
        {
            std::ifstream file(filename); // Открываем файл на чтение
            if (!file.is_open()) 
            {
                std::cerr << "[Serializer] Error: Cannot open file " << filename << std::endl;
                return {}; // Возвращаем пустой словарь при ошибке
            }

            // Проверка на пустой файл (чтобы избежать ошибок чтения)
            file.seekg(0, std::ios::end); // Перемещаем курсор в конец
            if (file.tellg() == 0) {
                return {}; // Файл пуст
            }
            file.seekg(0, std::ios::beg); // Возвращаем курсор в начало

            // Считываем всё содержимое файла в строковый буфер
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            
            std::string content = buffer.str();
            if (content.empty()) 
                return {};

            // Передаем прочитанную строку в парсер (deserialize)
            return deserialize(content);
        }
};