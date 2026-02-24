/*#include <iostream>
#include <string>
#include "userrepository.h"
#include "fileauthservice.h"

// Вспомогательная функция для красивого вывода разделителей
void print_section(const std::string& title) 
{
    std::cout << "\n========================================\n";
    std::cout << " " << title << "\n";
    std::cout << "========================================\n";
}

int main() 
{
    // Пути к файлам данных
    const std::string users_file = "users_db.txt";
    const std::string session_file = "session.txt";

    print_section("Инициализация системы");
    
    // Создаем репозиторий пользователей (загружает данные из файла, если он есть)
    UserRepository user_repo(users_file);
    
    // Создаем сервис авторизации, передавая ему ссылку на репозиторий
    FileAuthService auth_service(session_file, user_repo);

    // 1. Добавление пользователей
    print_section("1. Добавление пользователей");
    
    User user1(1, "Alice Smith", "alice_dev", "secret123", 
               std::optional<std::string>("alice@example.com"), 
               std::optional<std::string>("Moscow, Lenina st. 10"));
    
    User user2(2, "Bob Johnson", "bob_admin", "password_qwerty", 
               std::optional<std::string>("bob@example.com"), 
               std::nullopt); // Адрес не указан

    std::cout << "Добавляем Alice...\n";
    user_repo.add(user1);
    
    std::cout << "Добавляем Bob...\n";
    user_repo.add(user2);

    std::cout << "Список всех пользователей (отсортирован по имени):\n";
    auto all_users = user_repo.get_all();

    // Сортируем перед выводом, так как get_all возвращает копию вектора
    std::sort(all_users.begin(), all_users.end()); 
    for (const auto& u : all_users) 
        std::cout << u << "\n";

    // 2. Редактирование свойств пользователя
    print_section("2. Редактирование свойств пользователя");
    
    std::cout << "Меняем email у Alice...\n";
    user1.setEmail("alice.new@mail.com");
    user_repo.update(user1);

    auto updated_user = user_repo.get_by_id(1);
    if (updated_user.has_value()) 
    {
        std::cout << "Обновленные данные Alice:\n";
        std::cout << updated_user.value() << "\n";
    }

    // 3. Авторизация пользователя
    print_section("3. Авторизация пользователя");
    
    std::cout << "Попытка входа Alice...\n";
    auth_service.sign_in(user1);

    if (auth_service.is_authorized()) 
    {
        std::cout << "Статус: Авторизован.\n";
        auto current = auth_service.get_current_user();
        if (current.has_value()) 
            std::cout << "Текущий пользователь: " << current.value().getName() 
                      << " (Login: " << current.value().getLogin() << ")\n";   
    } 
    else 
    {
        std::cout << "Ошибка авторизации!\n";
    }

    // 4. Смена текущего пользователя
    print_section("4. Смена текущего пользователя");
    
    std::cout << "Выход Alice...\n";
    auth_service.sign_out(user1);
    
    std::cout << "Вход Bob...\n";
    auth_service.sign_in(user2);

    if (auth_service.is_authorized()) 
    {
        auto current = auth_service.get_current_user();
        if (current.has_value()) 
            std::cout << "Текущий пользователь теперь: " << current.value().getName() << "\n";
    }

    // Сохраняем состояние перед "перезапуском" (не делаем sign_out, чтобы сессия осталась в файле)
    std::cout << "\n[Имитация завершения работы программы без выхода из системы...]\n";
    std::cout << "Файл сессии содержит ID пользователя Bob.\n";

      // 5. Автоматическая авторизация при повторном заходе (имитация перезапуска)
    print_section("5. Автоматическая авторизация (Перезапуск программы)");
    
    std::cout << "Запуск новой инстанции сервиса (чтение файлов с диска)...\n";
    
    // Создаем новые объекты, имитируя свежий запуск программы.
    // Они прочитают те же файлы users_db.txt и session.txt.
    UserRepository restarted_repo(users_file);
    FileAuthService restarted_auth(session_file, restarted_repo);

    if (restarted_auth.is_authorized()) 
    {
        std::cout << "УСПЕХ: Автоматическая авторизация выполнена!\n";
        auto current = restarted_auth.get_current_user();
        if (current.has_value()) 
        {
            std::cout << "Восстановлен пользователь: " << current.value().getName() 
                      << " (ID: " << current.value().getId() << ")\n";
            std::cout << "Email: " << (current.value().getEmail().has_value() ? current.value().getEmail().value() : "нет") << "\n";
        }
    } 
    else 
    {
        std::cout << "Автоматическая авторизация не удалась (файл сессии пуст или поврежден).\n";
    }

    print_section("Завершение тестов");
    std::cout << "Готово. Проверьте файлы 'users_db.txt' и 'session.txt' в папке проекта.\n";
}

/* 2 отдельных main файла именно для перезапуска программы
#include <iostream>
#include <string>
#include <algorithm>
#include "userrepository.h"
#include "fileauthservice.h"

void print_section(const std::string& title) 
{
    std::cout << "\n========================================\n";
    std::cout << " " << title << "\n";
    std::cout << "========================================\n";
}

int main() 
{
    const std::string users_file = "users_db.txt";
    const std::string session_file = "session.txt";

    print_section("ЗАПУСК №1: Инициализация и тесты");
    std::cout << "Создаем базу пользователей и тестируем вход/выход...\n";

    UserRepository user_repo(users_file);
    FileAuthService auth_service(session_file, user_repo);

    // 1. Создание пользователей
    print_section("1. Добавление пользователей");
    User user1(1, "Alice Smith", "alice_dev", "secret123", 
               std::optional<std::string>("alice@example.com"), 
               std::optional<std::string>("Moscow, Lenina st. 10"));
    
    User user2(2, "Bob Johnson", "bob_admin", "password_qwerty", 
               std::optional<std::string>("bob@example.com"), 
               std::nullopt);

    user_repo.add(user1);
    user_repo.add(user2);
    std::cout << "Пользователи добавлены в базу.\n";

    // 2. Тест входа Alice
    print_section("2. Вход Alice");
    auth_service.sign_in(user1);
    if (auth_service.is_authorized()) 
        std::cout << "Alice успешно вошла.\n";

    // 3. Смена на Bob
    print_section("3. Смена пользователя на Bob");
    auth_service.sign_out(user1);
    auth_service.sign_in(user2);
    
    if (auth_service.is_authorized()) 
        std::cout << "Bob успешно вошел.\n";

    std::cout << "\n ЭТАП 1 ЗАВЕРШЕН.\n";
    std::cout << "Программа сейчас закроется. Файл сессии НЕ очищен!\n";
    std::cout << "ТЕПЕРЬ ЗАПУСТИ ПРОГРАММУ СНОВА (для этого нужно переключить файл main в настройках).\n";

    // ВАЖНО: Мы НЕ вызываем sign_out(). 
    // Файл session.txt останется с ID Боба (цифра 2).
    return 0;
}

// Файл: main_step2.cpp
#include <iostream>
#include <string>
#include "userrepository.h"
#include "fileauthservice.h"

void print_section(const std::string& title) 
{
    std::cout << "\n========================================\n";
    std::cout << " " << title << "\n";
    std::cout << "========================================\n";
}

int main() 
{
    const std::string users_file = "users_db.txt";
    const std::string session_file = "session.txt";

    print_section("ЗАПУСК №2: Проверка автоматического входа");
    std::cout << "Программа запущена повторно. Проверяем файл сессии...\n";

    // Создаем новые объекты. Они прочитают файлы с диска.
    UserRepository user_repo(users_file);
    FileAuthService auth_service(session_file, user_repo);

    // ПРОВЕРКА: Авторизован ли кто-то сразу после старта?
    if (auth_service.is_authorized()) 
    {
        std::cout << "УСПЕХ: Система обнаружила активную сессию!\n";
        
        auto current_user = auth_service.get_current_user();
        if (current_user.has_value()) 
        {
            std::cout << " Автоматический вход выполнен для: " 
                      << current_user.value().getName() 
                      << " (ID: " << current_user.value().getId() << ")\n";
            
            std::cout << "Email: " 
                      << (current_user.value().getEmail().has_value() ? current_user.value().getEmail().value() : "нет") 
                      << "\n";
        }
        
        std::cout << "\n Тест пройден. Очищаем сессию для следующего раза.\n";
        // Выходим, чтобы очистить файл сессии
        if (current_user.has_value())
            auth_service.sign_out(current_user.value
            юю=());
    } 
    else 
    {
        std::cout << "ОШИБКА: Сессия не найдена. Автоматический вход не сработал.\n";
        std::cout << "Убедитесь, что вы сначала запустили 'main_step1.cpp' и не прерывали его.\n";
    }

    return 0;
}*/

#include <iostream>
#include <string>
#include <optional>
#include <algorithm> 
#include "datarepository.h"

// 1. Хороший класс (Проходит проверку)
class GoodUser 
{
    private:

        int id_;
        std::string name_;
        
    public:
        // Конструктор по умолчанию нужен для чтения из файла (T item)
        GoodUser() : id_(0), name_("") {} 
        
        GoodUser(int id, const std::string& name) : id_(id), name_(name) {}
        
        // Метод getId() обязателен для концепта
        double getId() const 
        { 
            return id_; 
        }
        const std::string& getName() const 
        { 
            return name_; 
        }

        // Перегрузка оператора ввода (>>) нужна для load_from_file()
        friend std::istream& operator>>(std::istream& is, GoodUser& user) 
        {
            // Формат файла будет простым: ID|Name
            char sep; 
            return is >> user.id_ >> sep >> user.name_;
        }

        // Перегрузка оператора вывода (<<) нужна для save_to_file() и cout
        friend std::ostream& operator<<(std::ostream& os, const GoodUser& user) 
        {
            return os << user.id_ << "|" << user.name_;
        }
};

// 2. Плохой класс (Не проходит проверку)
class BadProduct 
{
    private:

        std::string code_;

    public:

        BadProduct(const std::string& code) : code_(code) {}

        // Ошибка: Нет метода getId(), возвращающего int
        std::string getCode() const 
        { 
            return code_; 
        }
};

int main() 
{
    // Тест 1: Хороший тип (GoodUser)
    std::cout << "1. Попытка создать репозиторий для GoodUser...\n";
    try 
    {
        DataRepository<GoodUser> userRepo("users_test.txt");
        
        GoodUser u1(1, "Alice");
        GoodUser u2(2, "Bob");
        
        userRepo.add(u1);
        userRepo.add(u2);
        
        std::cout << "УСПЕХ: Репозиторий создан и данные сохранены!\n";
        std::cout << "Прочитано из файла:\n";
        for(const auto& u : userRepo.get_all()) 
        {
            std::cout << "      - " << u.getName() << " (ID: " << u.getId() << ")\n";
        }
    } 
    catch (const std::exception& e)
     {
        std::cout << "Ошибка: " << e.what() << "\n";
    }

    std::cout << "\n";

    // Тест 2: Плохой тип (BadProduct)
    std::cout << "2. Попытка создать репозиторий для BadProduct...\n";

    // Этот код вызовет ошибку

    /*DataRepository<BadProduct> productRepo("products_test.txt");
    BadProduct p1("ABC-123");
    productRepo.add(p1);*/
}