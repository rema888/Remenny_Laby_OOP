#pragma once

#include "iloghandler.h"
#include <curl/curl.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#pragma comment(lib, "ws2_32.lib") // Линковка с библиотекой Winsock

// Вывод лога в консоль
class ConsoleHandler : public ILogHandler 
{
    public:

        void handle(LogLevel /*level*/, const std::string& text) const override 
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

        void handle(LogLevel /*level*/, const std::string& text) const override 
        {
            try 
            {
                // Поток для записи в файл
                std::ofstream file(file_path_, std::ios::app);
                if (file) 
                {
                    file << text << '\n';
                }
            } 
            catch (const std::exception& e) 
            {
                std::cerr << "FileHandler error: " << e.what() << "\n";
            }
        }
};

// SysLogHandler — отправляет логи в системный журнал Windows ("Просмотр событий")
class SysLogHandler : public ILogHandler 
{
    private:

        // HANDLE — тип Windows для дескрипторов (указателей на системные объекты)
        HANDLE h_event_log_ = nullptr;

    public:
        // Конструктор: регистрирует источник событий в системе
        explicit SysLogHandler(const std::string& source_name = "MyApp")
        {
            // RegisterEventSourceA — функция Windows API
            // nullptr — локальный компьютер
            // source_name.c_str() — имя источника (будет видно в "Просмотре событий")
            h_event_log_ = RegisterEventSourceA(nullptr, source_name.c_str());
            // Если регистрация не удалась, h_event_log_ останется nullptr
        }

        // Деструктор: освобождает ресурс — закрывает соединение с системой событий
        ~SysLogHandler() override 
        {
            // Если у нас есть валидный дескриптор
            if (h_event_log_) 
                DeregisterEventSource(h_event_log_); // Закрываем источник
        }

        // Основной метод: отправка лога в системный журнал
        void handle(LogLevel level, const std::string& text) const override 
        {
            // Если источник не зарегистрирован — выходим
            if (!h_event_log_) return;

            // Преобразуем LogLevel в тип события Windows
            // WORD — тип Windows: 16-битное целое число без знака
            WORD event_type;
            switch (level) 
            {
                case LogLevel::Info:  event_type = EVENTLOG_INFORMATION_TYPE; break;
                case LogLevel::Warn:  event_type = EVENTLOG_WARNING_TYPE;    break;
                case LogLevel::Error: event_type = EVENTLOG_ERROR_TYPE;      break;     
                default:              event_type = EVENTLOG_INFORMATION_TYPE;
            }

            // ReportEventA требует массив C-строк (const char* а не std::string)
            // .c_str() — метод, который даёт указатель на внутренние данные строки.
            const char* message = text.c_str();

            // Отправляем событие в системный журнал
            ReportEventA(
                h_event_log_,          // дескриптор источника
                event_type,            // тип события (INFO/WARN/ERROR)
                0,                     // категория (обычно не используется)
                0,                     // ID сообщения (если бы у нас был .mc-файл с шаблонами — был бы не 0)
                nullptr,               // SID пользователя (безопасность — не нужно)
                1,                     // сколько строк в массиве сообщений (у нас одна)
                0,                     // размер бинарных данных (у нас их нет)
                &message,              // указатель на массив строк (даже одна строка — передаётся как массив!)
                nullptr                // бинарные данные (не используем)
            );
        }
};

// SocketHandler — отправляет логи по TCP-сокету (например, на localhost:9999)
class SocketHandler : public ILogHandler 
{
    private:

        std::string host_;     // IP (например, "127.0.0.1")
        int port_;             // порт, на который будем подключаться (например, 9999)
        bool initialized_ = false; // флаг: инициализирован ли Winsock (В Windows перед использованием сокетов нужно инициализировать библиотеку Winsock)

    public:
        // Конструктор: инициализирует Winsock и сохраняет параметры подключения
        SocketHandler(const std::string& host, int port) : host_(host), port_(port) 
        {
            // WSADATA — структура, куда Windows запишет информацию о версии Winsock
            WSADATA wsaData;
            // WSAStartup — функция Windows для запуска библиотеки сокетов
            // MAKEWORD(2, 2) — просим использовать версию 2.2 (самая распространённая).
            // Если функция вернёт не 0 — значит, ошибка (например, нет сетевой поддержки)
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
            {
                std::cerr << "WSAStartup failed\n";
                return;
            }
            initialized_ = true; // Winsock успешно инициализирован
        }

        // Деструктор: завершает работу с Winsock
        ~SocketHandler() override 
        {
            if (initialized_) 
                WSACleanup(); // Освобождаем ресурсы Winsock
        }

        // Основной метод: подключается к сокету и отправляет сообщение
        void handle(LogLevel /*level*/, const std::string& text) const override 
        {
            if (!initialized_) return;

            // Создаём TCP-сокет
            SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock == INVALID_SOCKET) return;

            // Структура для адреса сервера
            sockaddr_in serverAddr{};
            serverAddr.sin_family = AF_INET; // IPv4
            serverAddr.sin_port = htons(static_cast<u_short>(port_)); // Порт в сетевой порядок байтов

            // Преобразуем имя хоста (например, "localhost") в IP-адрес
            ADDRINFOA hints{}, *result = nullptr;
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            if (getaddrinfo(host_.c_str(), nullptr, &hints, &result) != 0) 
            {
                closesocket(sock);
                return;
            }
            // Копируем IP-адрес из результата
            serverAddr.sin_addr = ((sockaddr_in*)result->ai_addr)->sin_addr;
            freeaddrinfo(result); // Освобождаем память getaddrinfo

            // Подключаемся к серверу
            if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) 
            {
                closesocket(sock);
                return;
            }

            // Отправляем сообщение + символ новой строки
            std::string message = text + "\n";
            send(sock, message.c_str(), static_cast<int>(message.size()), 0);

            // Закрываем сокет (соединение одноразовое)
            closesocket(sock);
        }
};
 
// FtpHandler — отправляет логи на FTP-сервер через libcurl
class FtpHandler : public ILogHandler 
{
    private:
        // URL вида: ftp://user:pass@host/log.txt
        std::string ftp_url_;

    public:
        // Конструктор: формирует FTP-URL и инициализирует libcurl
        FtpHandler(const std::string& host, const std::string& user, const std::string& pass)
            : ftp_url_("ftp://" + user + ":" + pass + "@" + host + "/log.txt")
        {
            // Глобальная инициализация libcurl (требуется один раз на всё приложение)
            curl_global_init(CURL_GLOBAL_ALL);
        }

        // Деструктор: завершает работу libcurl
        ~FtpHandler() override 
        {
            // Освобождение глобальных ресурсов libcurl
            curl_global_cleanup();
        }

        // Основной метод: загружает лог на FTP-сервер
        void handle(LogLevel /*level*/, const std::string& text) const override 
        {
            // Шаг 1: записываем сообщение во временный файл
            // (FTP в libcurl проще всего работать с файлами)
            std::ofstream tmp("temp_log.txt");
            tmp << text << "\n"; // Текст уже отформатирован — не добавляем ничего!
            tmp.close();

            // Шаг 2: инициализируем сессию curl
            CURL* curl = curl_easy_init();
            if (!curl) return;

            // Шаг 3: открываем временный файл для чтения
            FILE* fp = fopen("temp_log.txt", "rb");
            if (!fp) {
                curl_easy_cleanup(curl);
                return;
            }

            // Шаг 4: настраиваем параметры FTP-загрузки
            curl_easy_setopt(curl, CURLOPT_URL, ftp_url_.c_str());           // Куда загружать
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);                      // Режим загрузки (PUT)
            curl_easy_setopt(curl, CURLOPT_READDATA, fp);                    // Откуда читать данные
            curl_easy_setopt(curl, CURLOPT_INFILESIZE,                       // Размер файла
                (long)std::filesystem::file_size("temp_log.txt"));

            // Шаг 5: выполняем загрузку
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "FTP error: " << curl_easy_strerror(res) << "\n";
            }

            // Шаг 6: очистка
            fclose(fp);
            std::remove("temp_log.txt"); // Удаляем временный файл
            curl_easy_cleanup(curl);
        }
};