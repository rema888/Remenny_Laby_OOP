#pragma once

#include "iauthservice.h"
#include "userrepository.h"
#include <fstream>
#include <iostream>

class FileAuthService : public IAuthService 
{
    private:

        // Путь к файлу, где будет храниться ID текущего пользователя
        std::string session_file_;
        // Ссылка на репозиторий пользователей
        UserRepository& user_repo_; 

        // Вспомогательный метод для чтения ID из файла
        std::optional<int> read_session_id() const 
        {
            std::ifstream file(session_file_);
            if (!file.is_open()) 
                return std::nullopt;
            
            int id;

            if (file >> id) 
                return id;
            
            return std::nullopt;
        }

        // Вспомогательный метод для записи ID в файл
        void write_session_id(int id) 
        {
            std::ofstream file(session_file_); 
            
            if (!file.is_open()) 
                throw std::runtime_error("Ошибка открытия файла для записи");
            
            file << id; 
        }

        // Вспомогательный метод для очистки файла
        void clear_session() 
        {
            std::ofstream file(session_file_, std::ios::trunc);
            // std::ios::trunc очищает все содержимое в файле
        }

    public:

        explicit FileAuthService(const std::string& session_file, UserRepository& repo)
            : session_file_(session_file), user_repo_(repo) {}

        ~FileAuthService() override = default;

        void sign_in(const User& user) override 
        {
            // Проверяем, существует ли пользователь в базе данных
            auto existing_user = user_repo_.get_by_id(user.getId());
            if (!existing_user.has_value()) 
            {
                std::cout << "Ошибка: Пользователь с ID " << user.getId() << " не найден.\n";
                return;
            }

            // Записываем ID пользователя в файл
            write_session_id(user.getId());
            std::cout << "Пользователь " << user.getName() << " вошел в систему успешно.\n";
        }

        void sign_out(const User& user) override 
        {
            // Проверяем, что выходит именно тот, кто вошел
            auto current_id = read_session_id();
            if (current_id.has_value() && current_id.value() == user.getId()) 
            {
                clear_session();
                std::cout << "Пользователь " << user.getName() << " вышел из системы.\n";
            } 
            else
            {
                std::cout << "Предупреждение: Активная сессия не найдена или пользователь не совпадает.\n";
                // Даже если несоответствие, очищаем сессию на всякий случай, если она есть
                if (current_id.has_value()) 
                    clear_session();      
            }
        }

        bool is_authorized() const override 
        {
            return read_session_id().has_value();
        }

        std::optional<User> get_current_user() const override 
        {
            auto id_opt = read_session_id();
            if (!id_opt.has_value()) 
                return std::nullopt;

            // Ищем полные данные пользователя в репозитории по сохраненному ID
            return user_repo_.get_by_id(id_opt.value());
        }
};