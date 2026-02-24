#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <array>
#include <vector>
constexpr double M_PI = 3.14159265358979323846;
constexpr double eps_compare = 1e-10;
constexpr double eps_division= 1e-15;

class Angle
{
    private:

        double m_radians;

    public:

        // Конструктор из радиан (по умолчанию - 0)
        explicit Angle(double radians = 0.0)
        {
            m_radians = radians;
        }

        // Статический метод, который вызывает внутри конструктор из градусов
        static Angle fromDegrees(double degrees)
        {
            return Angle(degrees * M_PI / 180.0);
        }

        // Геттер для радиан
        double getRadians() const
        {
            return m_radians;
        }

        // Геттер для градусов
        double getDegrees() const
        {
            return m_radians * 180.0 / M_PI;
        }

        // Сеттер для радиан
        void setRadians(double radians)
        {
            m_radians = radians;
        }

        // Сеттер для градусов
        void setDegrees(double degrees)
        {
            m_radians = degrees * M_PI / 180.0;
        }
        
        // Для сравнения приведем углы к диапазону [0;2pi)
        double normalization(double rad) const
        {
            double res = std::fmod(rad, 2.0 * M_PI); // остаток от деления rad на 2pi
            if (res < 0)
                res += (2.0 * M_PI);
            return res;    
        }

        // Для AngleRange
        double normalizedValue() const
        {
            return normalization(m_radians);
        }

        int compare(const Angle& other) const
        {
            double n1 = normalization(m_radians);
            double n2 = normalization(other.m_radians);
            if (std::abs(n1 - n2) < eps_compare) // учитываем погрешность, т.к. double - неточный тип
                return 0;
            return (n1 < n2) ? -1 : 1;
        }

        // Операторы сравнения
        bool operator==(const Angle& other) const
        {
            return compare(other) == 0;
        }

        bool operator!=(const Angle& other) const
        {
            return !(*this == other);
        }

        bool operator<(const Angle& other) const
        {
            return compare(other) < 0;
        }

        bool operator>(const Angle& other) const
        {
            return other < *this;
        }

        bool operator<=(const Angle& other) const
        {
            return !(other < *this);
        }

        bool operator>=(const Angle& other) const
        {
            return !(*this < other);
        }

        // Операторы преобразования угла к float, int, str
        explicit operator float() const
        {
            return static_cast<float>(m_radians);
        }

        explicit operator int() const
        {
            return static_cast<int>(m_radians);
        }
        
        operator std::string() const
        {
            return std::to_string(m_radians) + " rad";
        }

        // Оператор +
        Angle operator+(const Angle& other) const
        {
            return Angle(m_radians + other.m_radians);
        }

        Angle operator+(double rad) const
        {
            return Angle(m_radians + rad);
        }

        Angle operator+(float rad) const
        {
            return Angle(m_radians + static_cast<double>(rad));
        }

        Angle operator+(int rad) const
        {
            return Angle(m_radians + static_cast<double>(rad));
        }

        // Оператор -
        Angle operator-(const Angle& other) const
        {
            return Angle(m_radians - other.m_radians);
        }

        Angle operator-(double rad) const
        {
            return Angle(m_radians - rad);
        }

        Angle operator-(float rad) const
        {
            return Angle(m_radians - static_cast<double>(rad));
        }

        Angle operator-(int rad) const
        {
            return Angle(m_radians - static_cast<double>(rad));
        }

        // Оператор *
        Angle operator*(double number) const
        {
            return Angle(m_radians * number);
        }

        Angle operator*(float number) const
        {
            return Angle(m_radians * static_cast<double>(number));
        }

        Angle operator*(int number) const
        {
            return Angle(m_radians * static_cast<double>(number));
        }

        // Оператор /
        Angle operator/(double number) const
        {
            if (std::abs(number) < eps_division)
                throw std::invalid_argument("Деление на ноль");
            return Angle(m_radians / number);
        }

        Angle operator/(float number) const
        {
            if (std::abs(number) < eps_division)
                throw std::invalid_argument("Деление на ноль");
            return Angle(m_radians / static_cast<double>(number));
        }

        Angle operator/(int number) const
        {
            if (number == 0)
                throw std::invalid_argument("Деление на ноль");
            return Angle(m_radians / static_cast<double>(number));
        }

        // Строковое представление
        std::string toString() const
        {
            return std::to_string(getDegrees()) + "°";
        }

        std::string repr() const
        {
            return "Angle(" + std::to_string(m_radians) + ")";
        }
};

class AngleRange
{
    private:

        Angle m_start;
        Angle m_end;
        bool m_start_in;
        bool m_end_in;

        // Проверяет, является ли промежуток прямым
        bool isDirect() const 
        {
            double s = m_start.normalizedValue();
            double e = m_end.normalizedValue();
            return e >= s - eps_compare;
        }

        // Проверяет, пустой ли промежуток пустым (вырожденным с исключающими границами)
        bool isEmpty() const 
        {
            return m_start == m_end && !m_start_in && !m_end_in;
        }

        // Разбивает оборачивающийся промежуток на два прямых
        std::pair<AngleRange, AngleRange> splitIntoDirect() const 
        {
            if (isDirect()) 
            {
                return { *this, AngleRange() };
            }

            // Нормализованные значения
            double s = m_start.normalizedValue();
            double e = m_end.normalizedValue();

            // Используем угол, чуть меньше 2π, чтобы избежать нормализации в 0 
            Angle endOfCircle(2.0 * M_PI - 1e-12); 

            // Первая часть: от начального угла до конца круга [s, 360°)
            AngleRange part1(m_start, endOfCircle, m_start_in, false);
            // Вторая часть: от начала круга до конечного угла [0°, e]
            AngleRange part2(Angle(0.0), m_end, true, m_end_in);

            // Возвращаем обе части
            return { part1, part2 };
        }

        // Статический метод - вычитаем A - B (прямые промежутки)
        static std::array<AngleRange, 2> subtractDirect(const AngleRange& A, const AngleRange& B) 
        {
            // Для создания пустого промежутка
            auto makeEmpty = []() 
            {
                return AngleRange(Angle(0.0), Angle(0.0), false, false);
            };

            // Если A пуст, результат пуст
            if (A.isEmpty()) 
            {
                return { makeEmpty(), makeEmpty() };
            }

            // Нормализованные границы A и B
            double s1 = A.m_start.normalizedValue();
            double e1 = A.m_end.normalizedValue();
            double s2 = B.m_start.normalizedValue();
            double e2 = B.m_end.normalizedValue();

            // Если B полностью левее или правее A - не пересекаются
            if (e2 < s1 - eps_compare || s2 > e1 + eps_compare) 
            {
                return { A, makeEmpty() };
            }

            // B покрывает A полностью
            if (s2 <= s1 + eps_compare && e2 >= e1 - eps_compare) 
            {
                // Подготавливаем результат (две точки или пусто)
                std::array<AngleRange, 2> res{ makeEmpty(), makeEmpty() };
                size_t i = 0; // индекс для заполнения

                // Проверяем, остаётся ли левая точка A
                if (A.m_start_in && !(B.m_start_in && std::abs(s1 - s2) < eps_compare)) 
                {
                    // Точка остаётся, если A включает её, а B — нет
                    res[i++] = AngleRange(A.m_start, A.m_start, true, true);
                }
                // Проверяем, остаётся ли правая точка A
                if (A.m_end_in && !(B.m_end_in && std::abs(e1 - e2) < eps_compare)) 
                {
                    res[i++] = AngleRange(A.m_end, A.m_end, true, true);
                }
                return res;
            }

            // Если B прилипает слева к A (начинается до/в начале A и заканчивается внутри)
            if (s2 <= s1 + eps_compare) 
            {
                // Остаётся правая часть: (e2, e1]
                return { AngleRange(Angle(e2), A.m_end, false, A.m_end_in), makeEmpty() };
            }

            // Если B прилипает справа к A (заканчивается после/в конце A и начинается внутри)
            if (e2 >= e1 - eps_compare) 
            {
                // Остаётся левая часть: [s1, s2)
                return { AngleRange(A.m_start, Angle(s2), A.m_start_in, false), makeEmpty() };
            }

            // Если B находится строго внутри A - разбиваем на два
            return 
            {
                // Левая часть: [s1, s2)
                AngleRange(A.m_start, Angle(s2), A.m_start_in, false),
                // Правая часть: (e2, e1]
                AngleRange(Angle(e2), A.m_end, false, A.m_end_in)
            };
        }

    public:

        // Основной конструктор
        AngleRange(const Angle& start, const Angle& end, bool start_in = true, bool end_in = true)
        {
            m_start = start;
            m_end = end;
            m_start_in = start_in;
            m_end_in = end_in;
        }

        AngleRange() : m_start(Angle(0.0)), m_end(Angle(0.0)), m_start_in(false), m_end_in(false) {}

        // Делегирующие конструкторы
        AngleRange(double start, double end, bool start_in = true, bool end_in = true):
            AngleRange(Angle(start), Angle(end), start_in, end_in) {}

        AngleRange(float start, float end, bool start_in = true, bool end_in = true):
            AngleRange(Angle(static_cast<double>(start)), Angle(static_cast<double>(end)), start_in, end_in) {}   

        AngleRange(int start, int end, bool start_in = true, bool end_in = true):
            AngleRange(Angle(static_cast<double>(start)), Angle(static_cast<double>(end)), start_in, end_in) {}     

        // Строковое представление
        std::string toString() const 
        {
            // Считаем промежуток "пустым", если он вырожденный и обе границы исключены
            if (m_start == m_end && !m_start_in && !m_end_in) {
                return "∅"; 
            }
            if (m_start == m_end) {
                return "{" + m_start.toString() + "}";
            }
            char start_bracket = m_start_in ? '[' : '(';
            char end_bracket = m_end_in ? ']' : ')';
            return std::string(1, start_bracket) + m_start.toString() + ", " + m_end.toString() + std::string(1, end_bracket);
        }

        std::string repr() const
        {
            return "AngleRange(" + m_start.repr() + ", " + m_end.repr() + ", "
                    + (m_start_in ? "true" : "false") + ", " + (m_end_in ? "true" : "false")  + ")";
        }

        // Получение длины промежутка
        double length() const
        {
            double start_norm = m_start.normalizedValue();
            double end_norm   = m_end.normalizedValue();
            if (end_norm >= start_norm)
                return end_norm - start_norm;
            else
                return (2.0 * M_PI - start_norm) + end_norm;
        }

        // Сравнение на эквивалентность
        bool operator==(const AngleRange& other) const
        {
            return (m_start == other.m_start) && (m_end == other.m_end) &&
                   (m_start_in == other.m_start_in) && (m_end_in == other.m_end_in);
        }    

        bool operator!=(const AngleRange& other) const
        {
            return !(*this == other);
        }

        // Счетчик включения граничных точек для сравнения
        int StartEndInCount() const
        {
            return (m_start_in ? 1 : 0) + (m_end_in ? 1 : 0);
        }

        // Сравниваем промежутки сначала по длине, потом по количеству включенных граничных точек, потом лексикографическое сравнение
        bool operator<(const AngleRange& other) const
        {
            double len1 = (*this).length();
            double len2 = other.length();
            if (std::abs(len1 - len2) > eps_compare)
                return len1 < len2;
            int inc1 = (*this).StartEndInCount();
            int inc2 = other.StartEndInCount();
            if (inc1 != inc2)
                return inc1 < inc2;
               
            if (m_start != other.m_start)
                return m_start < other.m_start;
            if (m_end != other.m_end)
                return m_end < other.m_end;
            if (m_start_in != other.m_start_in)
                return m_start_in < other.m_start_in;
            return m_end_in < other.m_end_in;
        } 

        bool operator>(const AngleRange& other) const
        {
            return other < *this;
        }

        bool operator<=(const AngleRange& other) const
        {
            return !(other < *this);
        }

        bool operator>=(const AngleRange& other) const
        {
            return !(*this < other);
        }

        // Проверка, содержится ли угол внутри промежутка
        bool contains(const Angle& angle) const
        {
            // Нормализуем все углы к диапазону [0, 2π)
            double start_n = m_start.normalizedValue();
            double end_n = m_end.normalizedValue();
            double x = angle.normalizedValue();
            // Вспомогательная функция для сравнения с погрешностью
            auto isEqual = [&](double a, double b) 
            { 
                return std::abs(a - b) < eps_compare; 
            };
            // Прямой промежуток
            if (end_n >= start_n - eps_compare)
            {
                bool left_ok = m_start_in ? (x > start_n - eps_compare || isEqual(x, start_n))
                                        : (x > start_n + eps_compare);
                bool right_ok = m_end_in ? (x < end_n + eps_compare || isEqual(x, end_n))
                                        : (x < end_n - eps_compare);
                return left_ok && right_ok;
            }
            // Оборачивающийся промежуток
            else
            {      
                bool inFirstPart = m_start_in ? (x > start_n - eps_compare || isEqual(x, start_n))
                                            : (x > start_n + eps_compare);
                bool inSecondPart = m_end_in ? (x < end_n + eps_compare || isEqual(x, end_n))
                                            : (x < end_n - eps_compare);
                return inFirstPart || inSecondPart;
            }
        }
        
        // Проверка, содержится ли промежуток внутри промежутка
        bool contains(const AngleRange& other) const
        {
            return contains(other.m_start) && contains(other.m_end);
        }

        // Объединение множеств: A ∪ B
        std::array<AngleRange, 2> operator+(const AngleRange& other) const 
        {
            // Возвращаем пустой промежуток
            auto makeEmpty = []() 
            {
                return AngleRange(Angle(0.0), Angle(0.0), false, false);
            };

            // Нормализованные значения
            double s1 = m_start.normalizedValue();
            double e1 = m_end.normalizedValue();
            double s2 = other.m_start.normalizedValue();
            double e2 = other.m_end.normalizedValue();

            // Если оба промежутка прямые
            if (e1 >= s1 && e2 >= s2) 
            {
                // Проверяет, пересекаются ли промежутки:
                // e1 < s2 - первый заканчивается до начала второго - нет пересечения,
                // e2 < s1 - второй заканчивается до начала первого - нет пересечения
                if (!(e1 < s2 - eps_compare || e2 < s1 - eps_compare)) 
                {
                    // Объединяем
                    double ns = std::min(s1, s2); // Минимальное из двух начал 
                    double ne = std::max(e1, e2); // Максимальный из двух концов 
                    // Если новое начало совпадает с началом this → берём флаг включения из this, иначе — из other
                    bool startIn = (std::abs(ns - s1) < eps_compare) ? m_start_in : other.m_start_in;
                    bool endIn = (std::abs(ne - e1) < eps_compare) ? m_end_in   : other.m_end_in;
                    return { AngleRange(Angle(ns), Angle(ne), startIn, endIn), makeEmpty() };
                }
            }

            // Если:хотя бы один промежуток оборачивающийся,или промежутки не пересекаются,
            // то возвращает два исходных промежутка как есть (без объединения)
            return { *this, other };
        }

        // Разность множеств: A \ B
        std::array<AngleRange, 2> operator-(const AngleRange& other) const 
        {
            // Вспомогательная функция для пустого промежутка
            auto makeEmpty = []() {
                return AngleRange(Angle(0.0), Angle(0.0), false, false);
            };

            // Разбиваем текущий промежуток на две прямые части (хвост и голова)
            auto [a1, a2] = this->splitIntoDirect();

            // Разбиваем вычитаемый промежуток на две прямые части
            auto [b1, b2] = other.splitIntoDirect();

            // Список для накопления результатов
            std::vector<AngleRange> results;

            // Добавить непустые промежутки из массива в results
            auto addIfNotEmpty = [&results](const std::array<AngleRange, 2>& arr) 
            {
                if (!arr[0].isEmpty()) 
                    results.push_back(arr[0]);
                if (!arr[1].isEmpty()) 
                    results.push_back(arr[1]);
            };

            // Вычитаем хвост из хвоста: a1 \ b1
            addIfNotEmpty(subtractDirect(a1, b1));

            // Вычитаем голову из головы: a2 \ b2 (только если обе части существуют)
            if (!a2.isEmpty() && !b2.isEmpty()) 
                addIfNotEmpty(subtractDirect(a2, b2));

            // Подготавливаем финальный результат (максимум 2 промежутка)
            std::array<AngleRange, 2> final{ makeEmpty(), makeEmpty() };

            // Копируем до двух первых результатов
            for (size_t i = 0; i < std::min(results.size(), size_t(2)); i++) 
            {
                final[i] = results[i];
            }
            return final;
        }
};

int main()
{
    Angle a1; // Создание по умолчанию
    std::cout << "a1: " << a1.getRadians() << " " << a1.getDegrees() << std::endl; // Проверка геттера для радиан и градусов
    Angle a2(M_PI); // Создание из радиан
    std::cout << "a2: " << a2.getRadians() << " " << a2.getDegrees() << std::endl;
    a2.setRadians(2 * M_PI); // Проверка сеттера для радиан
    std::cout << a2.getRadians() << std::endl;
    Angle a3 = Angle::fromDegrees(90.0); // Создание из градусов
    std::cout << "a3: " << a3.getRadians() << " " << a3.getDegrees() << std::endl;
    a3.setDegrees(270); // Проверка сеттера для градусов
    std::cout << a3.getDegrees() << std::endl;

    std::cout << "a1: " << a1.toString() << " | " << a1.repr() << std::endl; // str и repr
    std::cout << "a2: " << a2.toString() << " | " << a2.repr() << std::endl;
    std::cout << "a3: " << a3.toString() << " | " << a3.repr() << std::endl;

    std::cout << a1.toString() << " == " << a3.toString() << " : " << (a1 == a3) << std::endl; // Операторы сравнения
    std::cout << a1.toString() << " != " << a3.toString() << " : " << (a1 != a3) << std::endl;
    std::cout << a1.toString() << " < " << a3.toString() << " : " << (a1 < a3) << std::endl;
    std::cout << a1.toString() << " > " << a3.toString() << " : " << (a1 > a3) << std::endl;
    std::cout << a1.toString() << " <= " << a3.toString() << " : " << (a1 <= a3) << std::endl;
    std::cout << a1.toString() << " >= " << a3.toString() << " : " << (a1 >= a3) << std::endl;

    std::cout << a2.toString() << " + " << a3.toString() << " = " << (a2 + a3).toString() << std::endl; // + - * /
    std::cout << a3.toString() << " - " << a2.toString() << " = " << (a3 - a2).toString() << std::endl; 
    std::cout << a2.toString() << " * 2 = " << (a2 * 2).toString() << std::endl; 
    std::cout << a2.toString() << " / 2 = " << (a2 / 2).toString() << std::endl; 

    AngleRange ar1(Angle::fromDegrees(0), Angle::fromDegrees(90)); // Создание через градусы и проверка repr
    std::cout << "ar1: " << ar1.repr() << std::endl;
    AngleRange ar2(0.0, M_PI); // Создание через радианы (double) и проверка str
    std::cout << "ar2: " << ar2.toString() << std::endl;
    AngleRange ar3(0, 1, false, false); // Создание через радианы (int) и проверка включения промежутков
    std::cout << "ar3: " << ar3.toString() << std::endl;
   
    AngleRange ar4(Angle::fromDegrees(90), Angle::fromDegrees(180));
    std::cout << "ar4: " << ar4.toString() << std::endl;
    AngleRange ar5(Angle::fromDegrees(90), Angle::fromDegrees(180), true, false);
    std::cout << "ar5: " << ar5.toString() << std::endl;

    std::cout << ar4.toString() << " length = " << ar4.length() << std::endl; // Длина промежутка
    std::cout << ar4.toString() << " == " << ar5.toString() << " : " << (ar4 == ar5) << std::endl; // Операторы сравнения
    std::cout << ar4.toString() << " != " << ar5.toString() << " : " << (ar4 != ar5) << std::endl;
    std::cout << ar5.toString() << " < " << ar4.toString() << " : " << (ar5 < ar4) << std::endl;
    std::cout << ar5.toString() << " <= " << ar4.toString() << " : "  << (ar5 <= ar4) << std::endl;
    std::cout << ar4.toString() << " > " << ar5.toString() << " : "  << (ar4 > ar5) << std::endl;
    std::cout << ar4.toString() << " >= " << ar5.toString() << " : "  << (ar4 >= ar5) << std::endl;

    AngleRange ar6(Angle::fromDegrees(10), Angle::fromDegrees(40)); 
    AngleRange ar7(Angle::fromDegrees(10), Angle::fromDegrees(40), false, false);
    Angle test_angle = Angle::fromDegrees(40);
    std::cout << test_angle.toString() << " in " << ar6.toString() << " : " << ar6.contains(test_angle) << std::endl;  // Проверка вхождения угла
    std::cout << test_angle.toString() << " in " << ar7.toString() << " : " << ar7.contains(test_angle) << std::endl;

    AngleRange test_anglerange(Angle::fromDegrees(80), Angle::fromDegrees(50));
    std::cout << test_anglerange.toString() << " in " << ar6.toString() << " : " << ar6.contains(test_anglerange) << std::endl; // Проверка вхождения промежутка
    std::cout << test_anglerange.toString() << " in " << ar7.toString() << " : " << ar7.contains(test_anglerange) << std::endl;

    AngleRange C(Angle::fromDegrees(10), Angle::fromDegrees(50));
    AngleRange D(Angle::fromDegrees(20), Angle::fromDegrees(60), false, false);
    auto diff1 = C - D;
    std::cout << C.toString() << " - " << D.toString() << " = " << diff1[0].toString() << " ∪ " << diff1[1].toString() << std::endl;
    // [10, 40] - (10, 40) = {10} V {40}
}