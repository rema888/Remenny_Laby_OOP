#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

enum class Color
{
    Red, Green, Yellow
};

namespace ANSI
{
    const char* reset = "\033[0m";
    const char* red = "\033[31m";
    const char* green = "\033[32m";
    const char* yellow = "\033[33m";

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

        std::map<char, std::vector<std::string>> glyphs;

    public:
    
        bool LoadFromFile(const std::string& filename)
        {
            std::ifstream file(filename);
            if(!file.is_open())
                return false;

            std::string line;
            char currentChar = 0;    

            while (std::getline(file, line))
            {
                if (line.empty())
                    continue;

                if (line.size() == 1 && std::isalpha(line[0]))    
                {
                    currentChar = std::toupper(line[0]);
                    glyphs[currentChar] = std::vector<std::string>();
                } 
                
                else if (currentChar != 0) 
                {
                    glyphs[currentChar].push_back(line);
                }
            } 
            
            return true;
        }

        bool hasChar(char c) const 
        {
            return glyphs.count(std::toupper(c)) > 0;
        }

    const std::vector<std::string>& getChar(char c) const 
    {
        static const std::vector<std::string> empty;
        char upper = std::toupper(c);
        auto it = glyphs.find(upper);
        if (it != glyphs.end())
            return it->second;
        return empty;
    }

    int getHeight() const 
    {
        if (glyphs.empty()) 
            return 0;
        return glyphs.begin()->second.size();
    }
};

class Printer
{
    private:

        static Font font;
        Color currentColor;
        int currentRow;
        int currentCol;
        char currentSymbol;

    public:

        Printer(Color color, int row, int col, char symbol = '*')
        : currentColor(color), currentRow(row), currentCol(col), currentSymbol(symbol) {}

        // Метод печати для объекта
        void print(const std::string& text) 
        {
            Print(text, currentColor, currentRow, currentCol, currentSymbol);
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

        static void Print(const std::string& text, Color color, int row, int col, char symbol = '*')
        {
            int height = font.getHeight();
            if (height == 0) 
            {
                std::string simple;
                for (char c : text) simple += symbol;
                std::cout << ANSI::GetColor(color);
                std::cout << "\033[" << row << ";" << col << "H";
                std::cout << simple << ANSI::reset;
                return;
            }

            for (int lineIdx = 0; lineIdx < height; ++lineIdx) 
            {
                std::cout << ANSI::GetColor(color);
                std::cout << "\033[" << (row + lineIdx) << ";" << col << "H";

                for (char c : text) 
                {
                    if (font.hasChar(c)) 
                    {
                        std::string fontLine = font.getChar(c)[lineIdx];
                        // Заменяем '*' или '#' на указанный символ
                        for (char& ch : fontLine) 
                        {
                            if (ch == '*' || ch == '#') 
                                ch = symbol;  
                        }
                        std::cout << fontLine;
                    } 
                    else 
                    {
                        // Неизвестный символ — выводим пробелы такой же ширины
                        if (!font.getChar('A').empty()) 
                            std::cout << std::string(font.getChar('A')[0].size(), ' ');
                        else 
                            std::cout << " "; 
                    }
                    std::cout << " ";
                }
            }
            std::cout << ANSI::reset;
        }

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

    if (Printer::LoadFont("font5.txt")) 
    {
        Printer::Print("Static", Color::Red, 1, 7, '*'); // Статический вывод
    } 
    
    if (Printer::LoadFont("font5.txt")) 
    {
        Printer p(Color::Green, 6, 3, '#'); // Объектный вывод (RAII)
        p.print("RAII");
    } // деструктор вызовет сброс цвета 
}

Font Printer::font;