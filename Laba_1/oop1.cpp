#define _USE_MATH_DEFINES // Для числа Пи
#include <cmath>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
static constexpr double eps_compare = 1e-10;
static constexpr double eps_division= 1e-15;

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
            if (std::abs(n1 - n2) < eps_compare) // учитываем погрешность, т.к. без неё 0 != 2pi (double - неточный тип)
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

        // Операторы преобразования угла к int, str
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

    public:

        // Основной конструктор
        AngleRange(const Angle& start, const Angle& end, bool start_in = true, bool end_in = true)
        {
            m_start = start;
            m_end = end;
            m_start_in = start_in;
            m_end_in = end_in;
        }

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
            char start_bracket = m_start_in ? '[' : '(';
            char end_bracket = m_end_in ? ']' : ')';
            return std::string(1, start_bracket) + m_start.toString() + ", " + m_end.toString() + end_bracket;
        }

        std::string repr() const
        {
            return "AngleRange(" + m_start.repr() + ", " + m_end.repr() + ", "
                    + (m_start_in ? "true" : "false") + ", " + (m_end_in ? "true" : "false");
        }

        // Получение длины промежутка
        double length() const
        {
            double start_norm = m_start.normalization(m_start.getRadians());
            double end_norm = m_end.normalization(m_end.getRadians());
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
            double len1 = this->length();
            double len2 = other.length();
            if (std::abs(len1 - len2) > eps_compare)
                return len1 < len2;
            int inc1 = this->StartEndInCount();
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
            auto isEqual = [&](double a, double b) { return std::abs(a - b) < eps_compare; };
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

        // Сложение промежутков
        std::vector<AngleRange> operator+(const AngleRange& other) const
        {
            Angle start = m_start + other.m_start;
            Angle end = m_end + other.m_end;
            // Нормализуем итоговые углы
            double norm_start = start.normalization(start.getRadians());
            double norm_end   = end.normalization(end.getRadians());
            bool start_in = m_start_in && other.m_start_in;
            bool end_in = m_end_in && other.m_end_in;
            return {AngleRange(Angle(norm_start), Angle(norm_end), start_in, end_in)};
        }

        // Вычитание промежутков
        std::vector<AngleRange> operator-(const AngleRange& other) const
        {
            Angle start = m_start - other.m_end;
            Angle end = m_end - other.m_start;
            // Нормализуем итоговые углы
            double norm_start = start.normalization(start.getRadians());
            double norm_end   = end.normalization(end.getRadians());
            bool start_in = m_start_in && other.m_start_in;
            bool end_in = m_end_in && other.m_end_in;
            return {AngleRange(Angle(norm_start), Angle(norm_end), start_in, end_in)};
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

    std::cout << "a1 == a3 " << (a1 == a3) << std::endl; // Операторы сравнения
    std::cout << "a1 != a3 " << (a1 != a3) << std::endl;
    std::cout << "a1 < a3 " << (a1 < a3) << std::endl;
    std::cout << "a1 > a3 " << (a1 > a3) << std::endl;
    std::cout << "a1 <= a3 " << (a1 <= a3) << std::endl;
    std::cout << "a1 >= a3 " << (a1 >= a3) << std::endl;

    std::cout << "a2 + a3 = " << (a2 + a3).toString() << std::endl; // + - * /
    std::cout << "a3 - a2 = " << (a3 - a2).toString() << std::endl;
    std::cout << "a2 * 2 = " << (a2 * 2).toString() << std::endl;
    std::cout << "a2 / 2 = " << (a2 / 2).toString() << std::endl;

    AngleRange ar1(Angle::fromDegrees(0), Angle::fromDegrees(90)); // Создание через градусы и проверка repr
    std::cout << "ar1: " << ar1.repr() << std::endl;
    AngleRange ar2(0.0, M_PI); // Создание через радианы (double) и проверка str
    std::cout << "ar2: " << ar2.toString() << std::endl;
    AngleRange ar3(0, 1, false, false); // Создание через радианы (int) и проверка включения промежутков
    std::cout << "ar3: " << ar3.toString() << std::endl;
   
    AngleRange ar4(Angle::fromDegrees(90), Angle::fromDegrees(180));
    AngleRange ar5(Angle::fromDegrees(90), Angle::fromDegrees(180), true, false);
    std::cout << "ar4.length: " << ar4.length() << std::endl; // Длина промежутка
    std::cout << "ar4 == ar5: " << (ar4 == ar5) << std::endl; // Операторы сравнения
    std::cout << "ar4 != ar5: " << (ar4 != ar5) << std::endl;
    std::cout << "ar5 < ar4: " << (ar5 < ar4) << std::endl;
    std::cout << "ar5 <= ar4: " << (ar5 <= ar4) << std::endl;
    std::cout << "ar4 > ar5: " << (ar4 > ar5) << std::endl;
    std::cout << "ar4 >= ar5: " << (ar4 >= ar5) << std::endl;

    AngleRange ar6(Angle::fromDegrees(0), Angle::fromDegrees(90));
    AngleRange ar7(Angle::fromDegrees(60), Angle::fromDegrees(130));
    Angle test_angle = Angle::fromDegrees(40);
    std::cout << "40° in ar6? " << ar6.contains(test_angle) << std::endl;  // Проверка вхождения угла
    std::cout << "40° in ar7? " << ar7.contains(test_angle) << std::endl;
    AngleRange test_anglerange(Angle::fromDegrees(50), Angle::fromDegrees(80));
    std::cout << "small in ar6? " << ar6.contains(test_anglerange) << std::endl; // Проверка вхождения промежутка
    std::cout << "small in ar7? " << ar7.contains(test_anglerange) << std::endl;

    auto sum = ar6 + ar7; // Сложение промежутков
    std::cout << "ar6 + ar7 = " << sum[0].toString() << std::endl;
    auto diff = ar6 - ar7; // Вычитание промежутков
    std::cout << "ar6 - ar7 = " << diff[0].toString() << std::endl;
}