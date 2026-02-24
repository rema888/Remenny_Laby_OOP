#pragma once

#include "iuserrepository.h"
#include "datarepository.h"
#include <optional>
#include <string>

class UserRepository : public DataRepository<User>, public IUserRepository
{
    public:

        explicit UserRepository(const std::string& filename)
            : DataRepository<User>(filename) {}

        std::optional<User> get_by_login(const std::string& login) const override
        {
            // Вектор всех пользователей
            auto all_users = this->get_all();
            
            for (const auto& user : all_users) 
            {
                if (user.getLogin() == login) 
                    return user;     
            }
            
            return std::nullopt;
        }
};