#pragma once

#include "user.h"
#include <optional>

class IAuthService 
{
    public:

        // Вход в систему
        virtual void sign_in(const User& user) = 0;

        // Выход из системы
        virtual void sign_out(const User& user) = 0;

        // Авторизован ли пользователь
        virtual bool is_authorized() const = 0;

        // Возвращает полные данные текущего пользователя, если он авторизован
        virtual std::optional<User> get_current_user() const = 0;

        virtual ~IAuthService() = default;
};