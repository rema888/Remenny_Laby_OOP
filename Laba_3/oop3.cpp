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
    // Создаём фильтры
    auto level_filter = std::make_unique<LevelFilter>(LogLevel::WARN);
    auto text_filter = std::make_unique<SimpleLogFilter>(std::string("disk"));
    auto regex_filter = std::make_unique<ReLogFilter>(std::string(R"(full)"));

    // Собираем 3 фильтра в список
    std::vector<std::unique_ptr<ILogFilter>> filters;
    filters.push_back(std::move(level_filter));
    filters.push_back(std::move(text_filter));
    filters.push_back(std::move(regex_filter));

    // Создаём форматтеры 
    std::vector<std::unique_ptr<ILogFormatter>> formatters;
    formatters.push_back(std::make_unique<SimpleFormatter>());

    // Создаём обработчики
    std::vector<std::unique_ptr<ILogHandler>> handlers;
    // 1. Вывод в консоль
    handlers.push_back(std::make_unique<ConsoleHandler>());
    // 2. Запись в файл
    handlers.push_back(std::make_unique<FileHandler>("lab3_output.log"));
    // 3. Имитация отправки по сокету (сохранит в socket_localhost_9999.log)
    handlers.push_back(std::make_unique<SocketHandler>("localhost", 9999));
    // 4. Имитация системного лога (сохранит в /var/log/myapp/app.log или аналог)
    handlers.push_back(std::make_unique<SyslogHandler>());
    // 5. Имитация FTP-загрузки (сохранит в ftp_example.com_log.txt)
    handlers.push_back(std::make_unique<FtpHandler>("example.com", "user", "pass"));
 
    // Создаём логгер
    Logger logger(std::move(filters), std::move(formatters), std::move(handlers));

    // Тест
    // Этот лог не пройдёт: уровень INFO, а фильтр требует WARN
    logger.log_info("disk space ok");

    // Этот лог пройдет: WARN + содержит "disk" + содержит "full"
    logger.log_warn("disk almost full");

    // Этот лог не пройдёт: нет слова "disk"
    logger.log_warn("memory almost full");

    // Этот лог не пройдёт: уровень ERROR, а фильтр — только WARN
    logger.log_error("disk almost full");
}