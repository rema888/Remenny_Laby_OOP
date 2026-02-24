#include <iostream>
#include "toad.h"
#include "frog.h"
#include "printhandler.h"
#include "validationhandler.h"

int main() 
{
    // Создаём объекты
    Toad t("Vanya", "Forest", 5);
    Frog f("Tanya", "Green", 2.0);

    // Создаём обработчики
    PrintHandler print_handler;
    ValidationHandler validation_handler;

    // Подписываем Toad на события
    t.property_changed += &print_handler;          // Вывод после изменения
    t.property_changing += &validation_handler;    // Валидация до изменения

    // Подписываем Frog на события
    f.property_changed += &print_handler;          // Первый обработчик
    f.property_changed += &print_handler;          // Второй обработчик → будет 2 уведомления
    f.property_changing += &validation_handler;

    std::cout << "=== ТЕСТ 1: Успешное изменение строки ===\n";
    std::cout << "Было: " << t.get_name() << "\n";
    t.set_name("Bob");
    std::cout << "Стало: " << t.get_name() << " (ожидается Bob)\n\n";

    std::cout << "=== ТЕСТ 2: Валидация пустой строки (ОТМЕНА) ===\n";
    std::cout << "Было: " << f.get_color() << "\n";
    std::cout << "Попытка установить пустую строку...\n";
    f.set_color("");
    std::cout << "Стало: " << f.get_color() << " (ожидается Green, т.к. отменено)\n\n";

    std::cout << "=== ТЕСТ 3: Валидация имени с '_' (ОТМЕНА) ===\n";
    std::cout << "Было: " << t.get__address() << "\n";
    std::cout << "Попытка изменить свойство '_address'...\n";
    t.set__address("_private");
    std::cout << "Стало: " << t.get__address() << " (ожидается Forest, т.к. отменено)\n\n";

    std::cout << "=== ТЕСТ 4: Успешное изменение числа ===\n";
    std::cout << "Было: " << t.get_age() << "\n";
    t.set_age(10);
    std::cout << "Стало: " << t.get_age() << " (ожидается 10)\n\n";

    std::cout << "=== ТЕСТ 5: Множественная подписка (двойной вывод) ===\n";
    std::cout << "Изменяем вес лягушки (лог должен появиться 2 раза):\n";
    f.set_weight(0.5);

    std::cout << "=== ТЕСТ 6: Успешное изменение после неудачи ===\n";
    f.set_color("Blue");
    std::cout << "Цвет лягушки стал: " << f.get_color() << "\n\n";

    std::cout << "=== ТЕСТ 7: Отписка от событий ===\n";
    std::cout << "Отписываем Frog от событий (2 раза, так как было 2 подписки)...\n";
    f.property_changed -= &print_handler;
    f.property_changed -= &print_handler; 
    
    std::cout << "Меняем цвет на 'Red'. Лога быть НЕ должно:\n";
    f.set_color("Red");
    std::cout << "Цвет лягушки стал: " << f.get_color() << " (изменился, но логов нет)\n";
}