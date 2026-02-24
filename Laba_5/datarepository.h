#pragma once

#include "idatarepository.h"
#include <fstream>
#include <stdexcept>
#include <concepts>

template <typename T>
// Concept - набор требований к типу
concept HasGetId = requires(const T a) 
{
    // Если у типа T есть такой метод — проверка проходит этот этап
    // Результат, который он возвращает, должен быть int
    { a.getId() } -> std::same_as<int>;
};

template <HasGetId T> // Ограничение на тип
class DataRepository : public virtual IDataRepository<T> 
{
    private:

        // Имя файла, где будут храниться данные
        std::string filename_;
        // Вектор, который хранит все объекты
        std::vector<T> items_;

        void load_from_file() 
        {
            items_.clear(); // Очищаем текущий список в памяти
            std::ifstream file(filename_); 
            if (!file.is_open()) 
                return;
            
            T item;
            while (file >> item) // Читаем обьект из файла
                items_.push_back(item);
            file.close();
        }

        void save_to_file() 
        {
            std::ofstream file(filename_); 
            if (!file.is_open()) 
                throw std::runtime_error("Ошибка открытия файла для записи");
            
            for (const auto& item : items_) 
                file << item << "\n";
            file.close();
        }

    public:

        explicit DataRepository(const std::string& filename) : filename_(filename) 
        {
            load_from_file(); // При создании репозитория данные уже будут в памяти
        }

        ~DataRepository() override 
        {
            save_to_file(); // Сохраняем изменения перед выходом
        }

        std::vector<T> get_all() const override 
        {
            return items_;
        }

        std::optional<T> get_by_id(int id) const override 
        {
            for (const auto& item : items_) 
            {
                if (item.getId() == id) 
                    return item;   
            }
            return std::nullopt;
        }

        void add(const T& item) override 
        {
            // Сначала проверяем, нет ли уже такого элемента по ID
            for (const auto& stored : items_) 
            {
                if (stored.getId() == item.getId())        
                    return;      
            }

            items_.push_back(item);
            save_to_file();
        }

        void update(const T& item) override 
        {
            for (auto& stored_item : items_) 
            {
                if (stored_item.getId() == item.getId()) 
                {
                    stored_item = item;
                    save_to_file();
                    return;
                }
            }
            throw std::runtime_error("Обьект с таким Id не найден");
        }

        void remove(const T& item) override 
        {
            // find_if проходит по списку и находит первый элемент, удовлетворяющий условию
            auto it = std::find_if(items_.begin(), items_.end(), 
            // Поочередно сравниваем каждый элемент вектора (stored) с тем, что хотим удалить (item)
            [&item](const T& stored) 
            {
                return stored.getId() == item.getId();
            });

            if (it != items_.end()) 
            {
                items_.erase(it);
                save_to_file();
            }
        }
};