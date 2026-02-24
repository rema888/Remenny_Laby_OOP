#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

// Перечислитель enum 
enum class Color
{
    Red, Green, Yellow
};

// Пространство имен ANSI
namespace ANSI
{
    // ANSI-коды
    const std::string reset = "\033[0m";
    const std::string red = "\033[31m";
    const std::string green = "\033[32m";
    const std::string yellow = "\033[33m";

    // Связываем значение цвета с ANSI-строкой
    std::string GetColor(Color color)
    {
        switch(color)
        {
            case Color::Red: return red;
            case Color::Green: return green;
            case Color::Yellow: return yellow;
            default: return reset;
        }
    }
}

class Font
{
    private:

        // Словарь, который связывает символ с представлением в виде вектора строк
        std::map<char, std::vector<std::string>> glyphs; 

    public:
    
        bool LoadFromFile(const std::string& filename)
        {
            std::ifstream file(filename);
            // Удалось ли открыть файл
            if(!file.is_open())
                return false;

            std::string line;
            char currentChar = 0;    

            // Читаем файл построчно
            while (std::getline(file, line)) // Считываем одну строку из file и кладём её в line
            {
                if (line.empty())
                    continue;

                // Если строка из одного символа и является буквой    
                if (line.size() == 1 && std::isalpha(line[0]))    
                {
                    currentChar = line[0];
                    // Создаем пустой вектор строк для текущего символа 
                    glyphs[currentChar] = std::vector<std::string>();
                } 
                
                else if (currentChar != 0) 
                {
                    // Добавляем текущую строку в конец вектора
                    glyphs[currentChar].push_back(line);
                }
            } 
            // Файл был прочитан
            return true; 
        }

        // Есть ли эта буква в файле со шрифтом
        bool hasChar(char c) const 
        {
            return glyphs.count(std::toupper(c)) > 0;
        }

    // Получаем вектор строк для символа
    const std::vector<std::string>& getChar(char c) const 
    {
        static const std::vector<std::string> empty;
        char upper = std::toupper(c);
        // Ищем ключ буквы в словаре
        auto it = glyphs.find(upper);
        // Если символ найден, возвращаем вектор из пары ключ-значение (second)
        if (it != glyphs.end())
            return it->second;
        // Иначе вернем пустой вектор
        return empty;
    }

    // Высота шрифта
    int getHeight() const 
    {
        // Если ни одного символа не загружено
        if (glyphs.empty()) 
            return 0;
        // Берем первый символ и смотрим какой размер у его вектора строк
        return glyphs.begin()->second.size();
    }
};

class Printer
{
    private:

        static Font font;
        Color currentColor;
        // Номера строки и столбца в консоли, где будет начинаться вывод текста
        int currentRow;
        int currentCol;
        char currentSymbol;
        bool currentItalic;

    public:

        Printer(Color color, int row, int col, char symbol = '*', bool italic = false)
        : currentColor(color), currentRow(row), currentCol(col), currentSymbol(symbol), currentItalic(italic) {}

        // Метод печати для объекта
        void print(const std::string& text) 
        {
            Print(text, currentColor, currentRow, currentCol, currentSymbol, currentItalic);
        }

        // Деструктор: сбрасывает цвет консоли
        ~Printer() 
        {
            std::cout << ANSI::reset;
        }

        static bool LoadFont(const std::string& filename)
        {
            return font.LoadFromFile(filename);
        }

        // Статический метод для вывода в консоль 
        static void Print(const std::string& text, Color color, int row, int col, char symbol = '*', bool italic = false)
        {
            int height = font.getHeight();

            // Идем по строкам шрифта
            for (int lineIdx = 0; lineIdx < height; lineIdx++) 
            {
                int shift = italic ? (height - 1 - lineIdx) : 0; // 5 - 0, 4 - 1, 3 - 2, 2 - 3, 1 - 4

                // Устанавливаем цвет текста в консоли
                std::cout << ANSI::GetColor(color);

                // Перемещает курсор терминала в нужную позицию
                /*std::cout << "\033[" << (row + lineIdx) << ";" << col << "H";*/
                std::cout << "\033[" << (row + lineIdx) << ";" << (col + shift) << "H";

                for (char c : text) 
                {
                    // Есть ли этот символ в файле со шрифтом
                    if (font.hasChar(c)) 
                    {
                        // Получаем нужную строку из вектора строк
                        std::string fontLine = font.getChar(c)[lineIdx];
                        // Проходим по каждому символу в этой строке
                        for (char& ch : fontLine) 
                        {
                            if (ch == '*') 
                                ch = symbol;  
                        }
                        std::cout << fontLine;
                    } 
                    else 
                    {
                        // Неизвестный символ — выводим пробелы такой же ширины
                        if (!font.getChar('A').empty()) // Предполагаем, что буква A точно есть 
                            std::cout << std::string(font.getChar('A')[0].size(), ' ');
                        else // Если даже А нету
                            std::cout << " "; 
                    }
                    // После каждого символа выводим дополнительный пробел
                    std::cout << " ";
                }
            }
            std::cout << ANSI::reset;
        }

        // Статический метод для очистки экрана
        static void ClearScreen() 
        {
            std::cout << "\033[2J\033[H" << std::endl;
        }
};

int main()
{
    if (!Printer::LoadFont("font5.txt")) 
    {
        std::cerr << "Не удалось загрузить шрифт!\n";
        return 1;
    }
    
    Printer::ClearScreen();

    /* Printer::Print("Static", Color::Red, 1, 7, '*'); // Статический вывод 
    
    Printer p(Color::Green, 6, 3, '#'); // Объектный вывод
    p.print("Default");  */
    
    Printer p(Color::Green, 6, 3, '#', true); 
    p.print("Italic");  
}

// Создаем статическую переменную в глобальной области видимости
Font Printer::font;

/*
 *** 
*   *
*****  
*   *
*   *

    *** 
   *   *
  *****
 *   *
*   *
*/