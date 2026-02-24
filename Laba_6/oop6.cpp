#include "concretecommands.h"
#include "keyboard.h"
#include "keyboardstatesaver.h"
#include <iostream>

int main() 
{
    std::cout << "=== Starting Virtual Keyboard Lab ===" << std::endl;

    // Создаем клавиатуру с логированием в файл
    Keyboard keyboard("lab6_log.txt");

    // Создаем команды, передавая им ссылки на внутреннее состояние клавиатуры
    auto cmdA = std::make_shared<KeyCommand>(keyboard.getPrintedSeqRef(), 'a');
    auto cmdB = std::make_shared<KeyCommand>(keyboard.getPrintedSeqRef(), 'b');
    auto cmdC = std::make_shared<KeyCommand>(keyboard.getPrintedSeqRef(), 'c');
    auto cmdD = std::make_shared<KeyCommand>(keyboard.getPrintedSeqRef(), 'd');
    
    auto cmdVolUp = std::make_shared<VolumeUpCommand>(keyboard.getVolumeLevelRef());
    auto cmdVolDown = std::make_shared<VolumeDownCommand>(keyboard.getVolumeLevelRef());
    auto cmdPlayer = std::make_shared<MediaPlayerCommand>(keyboard.isPlayerRunningRef());

    // Привязываем команды к клавишам
    keyboard.bindKey("a", cmdA);
    keyboard.bindKey("b", cmdB);
    keyboard.bindKey("c", cmdC);
    keyboard.bindKey("d", cmdD);
    keyboard.bindKey("ctrl++", cmdVolUp);
    keyboard.bindKey("ctrl+-", cmdVolDown);
    keyboard.bindKey("ctrl+p", cmdPlayer);

    // Тестирование работы
    std::cout << "\n--- Test 1: Typing ---" << std::endl;
    keyboard.pressKey("a");
    keyboard.pressKey("b");
    keyboard.pressKey("c");
    
    std::cout << "\n--- Test 2: Undo/Redo ---" << std::endl;
    keyboard.undo();   // Отменяет 'c'
    keyboard.undo();   // Отменяет 'b'
    keyboard.redo();   // Возвращает 'b'
    
    std::cout << "\n--- Test 3: Multimedia ---" << std::endl;
    keyboard.pressKey("ctrl++"); // Громкость вверх
    keyboard.pressKey("ctrl+-"); // Громкость вниз
    keyboard.pressKey("ctrl+p"); // Плеер
    
    std::cout << "\n--- Test 4: More Typing ---" << std::endl;
    keyboard.pressKey("d");
    keyboard.undo();   // Отменяет 'd'
    keyboard.undo();   // Отменяет плеер

    // Сохранение состояния (ассоциаций)
    std::cout << "\n--- Test 5: Saving Configuration ---" << std::endl;
    KeyboardStateSaver saver("keyboard_config.json");
    saver.save(keyboard);

    // Эмуляция перезапуска программы
    std::cout << "\n=== Simulating Program Restart ===" << std::endl;
    
    // Создаем новую клавиатуру (состояние сброшено, буфер пустой)
    Keyboard newKeyboard("lab6_log_restarted.txt");
    
    // Загружаем конфигурацию
    saver.load(newKeyboard);

    // Проверяем, работают ли загруженные команды
    std::cout << "\n--- Test 6: Using Restored Bindings ---" << std::endl;
    std::cout << "Typing 'a' on restored keyboard:" << std::endl;
    newKeyboard.pressKey("a");
    newKeyboard.pressKey("b");
    
    std::cout << "\nProgram finished successfully." << std::endl;
}