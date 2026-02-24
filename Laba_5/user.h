#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <sstream>

class User 
{
    private:

        int id_;
        std::string name_;
        std::string login_;
        std::string password_;
        // Поля email и address являются необязательными, поэтому используем std::optional
        std::optional<std::string> email_;
        std::optional<std::string> address_;

    public:
        // Конструктор по умолчанию
        User() 
            : id_(0), name_(), login_(), password_(), email_(std::nullopt), address_(std::nullopt) {}

        // Основной конструктор
        explicit User(int id, 
                    std::string name, 
                    std::string login, 
                    std::string password,
                    std::optional<std::string> email = std::nullopt,
                    std::optional<std::string> address = std::nullopt)
            : id_(id), 
            name_(std::move(name)),
            login_(std::move(login)), 
            password_(std::move(password)),
            email_(std::move(email)), 
            address_(std::move(address)) {}

        // Геттеры
        int getId() const 
        { 
            return id_; 
        }
        const std::string& getName() const 
        { 
            return name_; 
        }
        const std::string& getLogin() const 
        { 
            return login_; 
        }
        const std::optional<std::string>& getEmail() const 
        { 
            return email_; 
        }
        const std::optional<std::string>& getAddress() const 
        { 
            return address_; 
        }

        // Сеттеры
        void setName(const std::string& name) 
        { 
            name_ = name; 
        }
        void setLogin(const std::string& login) 
        { 
            login_ = login; 
        }
        void setPassword(const std::string& password) 
        { 
            password_ = password; 
        }
        void setEmail(const std::optional<std::string>& email) 
        { 
            email_ = email; 
        }
        void setAddress(const std::optional<std::string>& address) 
        { 
            address_ = address; 
        }

        // Оператор сравнения < для сортировки по полю name
        bool operator<(const User& other) const 
        {
            return this->name_ < other.name_;
        }
        
        // Оператор вывода в поток
        friend std::ostream& operator<<(std::ostream& os, const User& user) 
        {
            os << user.id_ << "|"
            << user.name_ << "|"
            << user.login_ << "|"
            << user.password_ << "|"
            << (user.email_.has_value() ? "1|" + user.email_.value() + "|" : "0|")
            << (user.address_.has_value() ? "1|" + user.address_.value() : "0");

            return os;
        }
        
        // Оператор ввода в поток
        friend std::istream& operator>>(std::istream& is, User& user) 
        {
            std::string line;
            // 1. Читаем всю строку целиком
            if (!std::getline(is, line)) return is;
            
            // Если строка пустая или содержит только перевод строки - выходим
            if (line.empty() || line == "\r") return is;
            if (line.back() == '\r') line.pop_back(); // Убираем лишний символ Windows

            // Поток, внутри которого лежит наша строка line
            std::stringstream ss(line);
            
            // Временные переменные для данных
            std::string s_id, s_name, s_login, s_pass, s_has_email, s_email, s_has_addr, s_addr;

            // 2. Читаем части по очереди, разделяя палочкой '|'
            // Если какая-то часть не прочиталась, просто выходим
            if (!std::getline(ss, s_id, '|')) return is;
            if (!std::getline(ss, s_name, '|')) return is;
            if (!std::getline(ss, s_login, '|')) return is;
            if (!std::getline(ss, s_pass, '|')) return is;

            // Читаем флаг email
            if (!std::getline(ss, s_has_email, '|')) return is;

            // Читаем email, только если флаг говорит, что он есть
            if (s_has_email == "1") 
            {
                if (!std::getline(ss, s_email, '|')) return is;
            }

            // Читаем флаг адреса
            if (!std::getline(ss, s_has_addr, '|')) return is;

            // Читаем адрес, только если флаг говорит, что он есть
            if (s_has_addr == "1") 
            {
                // Адрес последний, читаем до конца строки
                if (!std::getline(ss, s_addr)) return is;
            }

            // 3. Присваиваем значения полям объекта
            std::stringstream id_stream(s_id); // Поток, внутри которого лежит строка с числом
            id_stream >> user.id_; // Превращает число в строку и записывает в user.id

            user.name_ = s_name;
            user.login_ = s_login;
            user.password_ = s_pass;

            if (s_has_email == "1") 
                user.email_ = s_email;
            else 
                user.email_ = std::nullopt;

            if (s_has_addr == "1") 
                user.address_ = s_addr;
            else 
                user.address_ = std::nullopt;

            return is;
        }
};