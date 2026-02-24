#include <C:/Users/Nitro5/vcpkg/installed/x64-mingw-dynamic/include/curl/curl.h> 
#include <iostream>
#include <memory>
#include <vector>
 
// Подключаем все заголовки
#include "loglevel.h"
#include "ilogfilter.h"
#include "logfilters.h"
#include "iloghandler.h"
#include "loghandlers.h"
#include "ilogformatter.h"
#include "simpleformatter.h"
#include "logger.h"

int main() 
{
    // Собираем 3 фильтра в список
    std::vector<std::unique_ptr<ILogFilter>> filters;
    filters.push_back(std::make_unique<LevelFilter>(LogLevel::Warn));
    filters.push_back(std::make_unique<SimpleLogFilter>("disk"));
    filters.push_back(std::make_unique<ReLogFilter>(R"(full)"));

    // Создаём форматтер
    std::vector<std::unique_ptr<ILogFormatter>> formatters;
    formatters.push_back(std::make_unique<SimpleFormatter>());

    // Создаём обработчики
    std::vector<std::unique_ptr<ILogHandler>> handlers;
    // 1. Вывод в консоль
    handlers.push_back(std::make_unique<ConsoleHandler>());
    // 2. Запись в файл
    handlers.push_back(std::make_unique<FileHandler>("lab3_output.log"));
    // 3. Сокет
    handlers.push_back(std::make_unique<SocketHandler>("127.0.0.1", 9999));
    // 4. Системный лог (eventvwr.msc)
    handlers.push_back(std::make_unique<SysLogHandler>());
    // 5. FTP-загрузка
    handlers.push_back(std::make_unique<FtpHandler>("127.0.0.1", "User1", "pass"));
 
    // Создаём логгер
    Logger logger(std::move(filters), std::move(formatters), std::move(handlers));

    // WARN + содержит "disk" + содержит "full"
    logger.log_warn("disk almost full");

    // Нет слова "disk"
    logger.log_info("memory almost full");

    // Уровень ERROR, а фильтр — только WARN
    logger.log_error("disk almost full");
}