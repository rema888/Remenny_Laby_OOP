#pragma once

#include <vector>
#include <optional>

template <typename T>
class IDataRepository 
{
    public:

        // Возвращает список объектов, которые есть в хранилище
        virtual std::vector<T> get_all() const = 0;

        // Ищет конкретный объект по его ID
        virtual std::optional<T> get_by_id(int id) const = 0;

        // Добавляет обьект в хранилище
        virtual void add(const T& item) = 0;

        // Обновляет старые данные обьекта на новые
        virtual void update(const T& item) = 0;

        // Удаляет обьект из хранилища
        virtual void remove(const T& item) = 0;

        virtual ~IDataRepository() = default;
};