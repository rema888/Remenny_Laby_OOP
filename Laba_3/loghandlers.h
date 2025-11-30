#pragma once

#include "iloghandler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>


// Вывод лога в консоль
class ConsoleHandler : public ILogHandler 
{
    public:

        void handle(LogLevel /*level*/, const std::string& text) override 
        {
            std::cout << text << std::endl;
        }
};

// Запись лога в файл
class FileHandler : public ILogHandler 
{
    private: 

        std::string file_path_;

    public:

        explicit FileHandler(const std::string& file_path) : file_path_(file_path) {}

        void handle(LogLevel /*level*/, const std::string& text) override 
        {
            std::ofstream file(file_path_, std::ios::app);
            if (file) 
            {
                file << text << '\n';
            }
        }
};

// Имитация записи в системные логи
class SyslogHandler : public ILogHandler 
{
    private: 

        std::string log_file_;

    public:

        SyslogHandler(const std::string& log_dir = "/var/log/myapp", 
                    const std::string& app_name = "app") 
        {
            std::filesystem::create_directories(log_dir); // создаём папку
            log_file_ = log_dir + "/" + app_name + ".log";
        }

        void handle(LogLevel /*level*/, const std::string& text) override 
        {
            std::ofstream file(log_file_, std::ios::app);
            if (file) 
            {
                file << text << '\n';;
            }
        }
};

// Имитация: пишет в локальный файл вместо сокета
class SocketHandler : public ILogHandler 
{
    private: 

        std::string log_file_;

    public:

        SocketHandler(const std::string& host, int port) 
        {
            log_file_ = "socket_" + host + "_" + std::to_string(port) + ".log";
        }

        void handle(LogLevel /*level*/, const std::string& text) override 
        {
            std::ofstream file(log_file_, std::ios::app);
            if (file) 
            {
                file << "[SOCKET MOCK] " << text << '\n';
            }
        }
};

// Имитация: сохраняет локально, как "загруженный файл"
class FtpHandler : public ILogHandler 
{
    private: 

        std::string log_file_;

    public:

        FtpHandler(const std::string& host, const std::string& /*user*/, const std::string& /*pass*/) 
        {
            log_file_ = "ftp_" + host + "_log.txt";
        }

        void handle(LogLevel /*level*/, const std::string& text) override 
        {
            std::ofstream file(log_file_, std::ios::app);
            if (file) 
            {
                file << "[FTP MOCK] " << text << '\n';
            }
        }
};