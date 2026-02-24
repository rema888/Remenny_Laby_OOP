#pragma once

#include "idatarepository.h"
#include "user.h"
#include <string>
#include <optional>

class IUserRepository : public virtual IDataRepository<User>
{
    public:

        // Ищет пользователя по логину
        virtual std::optional<User> get_by_login(const std::string& login) const = 0;

        virtual ~IUserRepository() override = default;
};