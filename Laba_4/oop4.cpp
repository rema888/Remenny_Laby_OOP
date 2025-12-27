#include <iostream>
#include "toad.h"
#include "frog.h"
#include "printhandler.h"
#include "validationhandler.h"

int main() 
{
    // Создаём объекты
    Toad t("Vanya", "Forest", 5);
    Frog f("Tanya", "Green", 2);

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

    std::cout << "Изменяем name y Toad на 'Bob' (должно пройти):\n";
    t.set_name("Bob");

    std::cout << "Пытаемся изменить color y Frog на '' (должно быть отменено):\n";
    f.set_color("");

    std::cout << "Пытаемся изменить _address y Toad на '_private' (имя c '_' (должно быть отменено):\n";
    t.set__address("_private");

    std::cout << "Изменяем age y Toad на 10 (число, должно пройти):\n";
    t.set_age(10);

    std::cout << "Изменяем weight y Frog на 0.5 (должно вывестись дваджды):\n";
    f.set_weight(0.5);

    std::cout << "Изменяем color y Frog на 'Blue' (непустая строка — пройдёт):\n";
    f.set_color("Blue");

    std::cout << "Отписываем Frog от property_changed и меняем цвет (вывода быть не должно):\n";
    f.property_changed -= &print_handler;
    f.property_changed -= &print_handler; 
    f.set_color("Red");
}