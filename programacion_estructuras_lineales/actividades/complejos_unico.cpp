
#include <cmath>
#include <type_traits>
#include <iostream>


template<typename T>
class Complex {
    static_assert(std::is_floating_point<T>::value,
                  "Value type must be float, double, or long double");
    T re_,   // parte real       -> Re(z)
      im_;   // parte imaginaria -> Im(z)

public:
    
    Complex(T const& re = T{}, T const& im = T{})
        : re_{re}, im_{im}   // lista de inicialización
    { /* cuerpo del constructor vacío */ }

    
    T const& real() const { return re_; }   // lectura  (objetos const)
    T&       real()       { return re_; }   // lectura/escritura (no const)

    // --- acceso a la parte imaginaria ---
    T const& imag() const { return im_; }   // lectura  (objetos const)
    T&       imag()       { return im_; }   // lectura/escritura (no const)

   
    T norm() const { return re_*re_ + im_*im_; }
    T arg() const { return std::atan2(im_, re_); }

    Complex conj() const { return {re_, -im_}; }


    // z1 += z2  ->  (x1 + x2, y1 + y2)
    Complex& operator+=(Complex const& z)
    {
        re_ += z.re_;   // suma de partes reales
        im_ += z.im_;   // suma de partes imaginarias
        return *this;   // devuelve el propio objeto ya modificado
    }

    // z += t (t real)  ->  (x + t, y)
    // Un real t equivale al complejo (t, 0), por eso solo cambia re_.
    Complex& operator+=(T const& t)
    {
        re_ += t;       // im_ permanece sin modificar
        return *this;
    }

    // ---------------------------- RESTA -----------------------------------

    // z1 -= z2  ->  (x1 - x2, y1 - y2)
    Complex& operator-=(Complex const& z)
    {
        re_ -= z.re_;
        im_ -= z.im_;
        return *this;
    }

    // z -= t (t real)  ->  (x - t, y)
    Complex& operator-=(T const& t)
    {
        re_ -= t;       // im_ permanece sin modificar
        return *this;
    }

    Complex& operator*=(Complex const& z)
    {
        T const r = re_*z.re_ - im_*z.im_;   // nueva parte real (temporal)
        im_ = re_*z.im_ + im_*z.re_;         // nueva parte imaginaria
        re_ = r;                             // ahora sí se actualiza re_
        return *this;
    }

    // z *= t (t real)  ->  (t*x, t*y)   (producto externo por escalar)
    Complex& operator*=(T const& t)
    {
        re_ *= t;
        im_ *= t;
        return *this;
    }
};  // <- el ';' tras la llave de cierre de una clase es OBLIGATORIO en C++



// Complex + Complex
template<typename T> inline
Complex<T> operator+(Complex<T> const& x, Complex<T> const& y) noexcept
{
    auto r = x;   // copia de x
    r += y;       // usa Complex& operator+=(Complex const&)
    return r;
}

// Complex + escalar
template<typename T> inline
Complex<T> operator+(Complex<T> const& x, T const& y) noexcept
{
    auto r = x;
    r += y;       // usa Complex& operator+=(T const&)
    return r;
}


template<typename T> inline
Complex<T> operator+(T const& x, Complex<T> const& y) noexcept
{
    auto r = y;   // copia del operando complejo
    r += x;
    return r;
}


// Complex - Complex
template<typename T> inline
Complex<T> operator-(Complex<T> const& x, Complex<T> const& y) noexcept
{
    auto r = x;
    r -= y;
    return r;
}

// Complex - escalar
template<typename T> inline
Complex<T> operator-(Complex<T> const& x, T const& y) noexcept
{
    auto r = x;
    r -= y;
    return r;
}


template<typename T> inline
Complex<T> operator-(T const& x, Complex<T> const& y) noexcept
{
    Complex<T> r{x};   // r = (x, 0)
    r -= y;
    return r;
}


template<typename T> inline
Complex<T> operator*(Complex<T> const& x, Complex<T> const& y) noexcept
{
    auto r = x;
    r *= y;
    return r;
}

// Complex * escalar
template<typename T> inline
Complex<T> operator*(Complex<T> const& x, T const& y) noexcept
{
    auto r = x;
    r *= y;
    return r;
}

// escalar * Complex
// El producto SÍ es conmutativo, así que se copia el complejo.
template<typename T> inline
Complex<T> operator*(T const& x, Complex<T> const& y) noexcept
{
    auto r = y;
    r *= x;
    return r;
}

template<typename T> inline
bool operator==(Complex<T> const& x, Complex<T> const& y)
{
    return x.real() == y.real() && x.imag() == y.imag();
}

// Complex == escalar
template<typename T> inline
bool operator==(Complex<T> const& x, T const& t)
{
    return x.real() == t && x.imag() == T{};   // T{} es el cero de tipo T
}

// escalar == Complex
template<typename T> inline
bool operator==(T const& t, Complex<T> const& y)
{
    return t == y.real() && T{} == y.imag();
}

template<typename T> inline
bool operator!=(Complex<T> const& x, Complex<T> const& y)
{
    return !(x == y);
}

// Complex != escalar
template<typename T> inline
bool operator!=(Complex<T> const& x, T const& t)
{
    return !(x == t);
}

// escalar != Complex
template<typename T> inline
bool operator!=(T const& t, Complex<T> const& y)
{
    return !(t == y);
}

template<typename T>
void mostrar(char const* nombre, Complex<T> const& z)
{
    std::cout << nombre << " = (" << z.real() << ", " << z.imag() << ")\n";
}

int main()
{
    // ------------------------------------------------------------------------
    // 1. Modificadores de acceso
    // ------------------------------------------------------------------------
    Complex<double> z{3.0, 2.0};   // crea el número complejo z = (3,2)
    // z.re_ = 1.0;                // ERROR: 're_' is private
    mostrar("z", z);

    // ------------------------------------------------------------------------
    // 2. Constructor con valores por defecto
    // ------------------------------------------------------------------------
    Complex<double> a,             // a.re_ = a.im_ = 0 por defecto
                    b{3.0};        //         b.im_ = 0 por defecto
    mostrar("a", a);               // (0, 0)
    mostrar("b", b);               // (3, 0)

    // ------------------------------------------------------------------------
    // 3 y 4. Interfaz pública y especificador const
    // ------------------------------------------------------------------------
    Complex<double> p{1.0, 3.0};         // p = (1,3) no constante
    p.real() = 5.0;                      // p = (5,3) ahora (versión T& real())
    double i = p.imag();                 // i = 3.0
    Complex<double> c = p.conj();        // c = (5,-3)
    Complex<double> const q{2.0, 4.0};   // q = (2,4) CONSTANTE
    double n = q.norm();                 // n = 20.0 (norm() es const: OK)
    // q.imag() = 7.0;                   // ERROR: assignment of read-only location
                                         // (q es const -> se usa T const& imag() const)
    mostrar("p", p);
    std::cout << "i = " << i << '\n';
    mostrar("c", c);
    std::cout << "n = " << n << '\n';
    std::cout << "arg(q) = " << q.arg() << " rad\n";

    // ------------------------------------------------------------------------
    // 5. Clase genérica: distintos tipos de coma flotante
    // ------------------------------------------------------------------------
    Complex<float>       fa{3.0f, 4.0f};    // OK, fa = (3,4)   (sufijo f -> float)
    Complex<double>      fb{8.0, -3.0};     // OK, fb = (8,-3)
    Complex<long double> fc{-6.0L, 5.0L};   // OK, fc = (-6,5)  (sufijo L -> long double)
    // Complex<char>     fd{'r', 's'};      // ERROR: static_assert -> char no es floating_point
    mostrar("fa", fa);
    mostrar("fb", fb);
    mostrar("fc", fc);
    // sizeof devuelve el tamaño en bytes de cada tipo (depende de la plataforma)
    std::cout << "sizeof(Complex<float>)       = " << sizeof(fa) << " bytes\n"
              << "sizeof(Complex<double>)      = " << sizeof(fb) << " bytes\n"
              << "sizeof(Complex<long double>) = " << sizeof(fc) << " bytes\n";

    // ------------------------------------------------------------------------
    // 6. Sobrecarga de operadores de asignación compuesta
    // ------------------------------------------------------------------------
    Complex<double> x{2, 3},
                    y{1, 4};
    x *= y;   // x = (2,3)*(1,4) = (2-12, 8+3) = (-10,11) ahora
    y += x;   // y = (1,4)+(-10,11) = (-9,15) ahora
    mostrar("x", x);
    mostrar("y", y);

    // ------------------------------------------------------------------------
    // 6. Operadores aritméticos binarios externos
    // ------------------------------------------------------------------------
    Complex<double> u{2.0, 3.0},   // u = (2,3)
                    v{1.0, 4.0};   // v = (1,4)
    // Paso a paso:
    //   3.0*v           = (3,12)          -> escalar * Complex
    //   2.0 + (3,12)    = (5,12)          -> escalar + Complex
    //   u*(5,12)        = (10-36, 24+15) = (-26,39)  -> Complex * Complex
    //   1.0 + (-26,39)  = (-25,39)        -> escalar + Complex
    auto s = 1.0 + u*(2.0 + 3.0*v);   // s = (-25,39), de tipo Complex<double>
    mostrar("s", s);

    auto d = 10.0 - u;                // [COMPLETADO] (10-2, -3) = (8,-3)
    mostrar("10 - u", d);

    // ------------------------------------------------------------------------
    // 6. Operadores de relación
    // ------------------------------------------------------------------------
    Complex<double> r1{1.0, 2.0},
                    r2{3.0, -4.0};
    // std::boolalpha hace que los bool se impriman como true/false en vez
    // de 1/0. Los paréntesis en (r1 == r2) son necesarios porque '<<' tiene
    // mayor precedencia que '=='.
    std::cout << std::boolalpha
              << "r1 == r2   -> " << (r1 == r2)  << '\n'   // false
              << "r1 != r2   -> " << (r1 != r2)  << '\n'   // true
              << "b  == 3.0  -> " << (b == 3.0)  << '\n'   // true  (b = (3,0))
              << "3.0 == z   -> " << (3.0 == z)  << '\n';  // false (z = (3,2))

    return 0;
}
