/* ============================================================================
 *  fraccion.cpp  —  TAD Fraccion<T> (números racionales) en un único fichero
 *  ---------------------------------------------------------------------------
 *  Contiene, en este orden:
 *    PARTE 1: el TAD Fraccion<T>
 *    PARTE 2: la función main() con ejemplos de uso
 *
 *  Recordatorio matemático:
 *    - Un número racional es un cociente n/d de dos enteros con d != 0.
 *    - Suma:      a/b + c/d = (a*d + c*b) / (b*d)
 *    - Resta:     a/b - c/d = (a*d - c*b) / (b*d)
 *    - Producto:  a/b * c/d = (a*c) / (b*d)
 *    - Cociente:  (a/b) / (c/d) = (a/b) * (d/c)      (solo si c != 0)
 *    - Muchas parejas representan el MISMO número: 1/2 = 2/4 = -3/-6 ...
 *
 *  ¿QUÉ APORTA RESPECTO A Complex<T>?
 *    Complex<T> aceptaba CUALQUIER pareja (re, im): no había valores
 *    "prohibidos". Una fracción, en cambio, tiene un INVARIANTE DE CLASE:
 *    una condición que TODO objeto debe cumplir SIEMPRE, desde que se
 *    construye hasta que se destruye. Aquí el invariante es la FORMA CANÓNICA:
 *
 *        (I1) den_ != 0                    (no existe x/0)
 *        (I2) den_ > 0                     (el signo va en el numerador)
 *        (I3) mcd(|num_|, den_) == 1       (fracción irreducible)
 *
 *    Así, cada número racional tiene UNA ÚNICA representación interna:
 *        Fraccion<int>{6, -8}  se guarda como  -3/4
 *        Fraccion<int>{0, 5}   se guarda como   0/1
 *
 *    Mantener el invariante es la VERDADERA razón de ser del encapsulamiento:
 *    si los atributos fueran públicos, cualquiera podría escribir f.den_ = 0
 *    y el objeto quedaría corrupto. Por eso aquí NO hay versiones no-const
 *    de num() y den() (en Complex sí había un T& real()).
 *
 *  Conceptos nuevos que aparecen (marcados con [NUEVO] en los comentarios):
 *    - Invariante de clase y función miembro PRIVADA de ayuda (normalizar)
 *    - Restricción de la plantilla a enteros con signo
 *    - Excepciones (throw / try / catch) para operaciones no válidas
 *    - Funciones amigas (friend) definidas dentro de la clase
 *    - Operador unario -  y operadores de orden  <  >  <=  >=
 *    - Sobrecarga de operator<< para imprimir con std::cout
 *    - Uso de la fracción con algoritmos de la biblioteca estándar (std::sort)
 *
 *  Compilación (requiere C++17 por std::gcd y std::lcm):
 *      g++ -std=c++17 -Wall -Wextra -o fraccion fraccion.cpp
 *  En CLion: crear proyecto "C++ Executable" con estándar C++17 y pegar este
 *  fichero completo en main.cpp.
 * ==========================================================================*/

// <numeric> aporta std::gcd (máximo común divisor) y std::lcm (mínimo común
// múltiplo), ambos desde C++17. Los usaremos para simplificar fracciones y
// para sumar con el mínimo denominador común.
#include <numeric>

// <type_traits> aporta std::is_integral<T> y std::is_signed<T>, rasgos de
// tipo que se evalúan EN TIEMPO DE COMPILACIÓN (igual que en Complex).
#include <type_traits>

// <stdexcept> aporta las clases de excepción estándar std::invalid_argument
// y std::domain_error, que lanzaremos ante operaciones no válidas.
#include <stdexcept>

// <ostream> aporta std::ostream, el tipo de std::cout, necesario para
// sobrecargar operator<<.
#include <ostream>

// <iostream>, <vector> y <algorithm> solo se usan en main() (PARTE 2) para
// mostrar resultados, guardar varias fracciones y ordenarlas con std::sort.
#include <iostream>
#include <vector>
#include <algorithm>


/* ############################################################################
 *  PARTE 1: TAD Fraccion<T>
 * ##########################################################################*/

/* ----------------------------------------------------------------------------
 *  template<typename T>
 *  ---------------------------------------------------------------------------
 *  Igual que Complex<T>, Fraccion<T> no es una clase sino una PLANTILLA: una
 *  familia de clases. Fraccion<int>, Fraccion<long> y Fraccion<long long>
 *  son tipos distintos. Elegir un T más grande permite numeradores y
 *  denominadores más grandes antes de que se produzca DESBORDAMIENTO
 *  (overflow): un int suele llegar a ~2.100 millones; un long long a ~9·10^18.
 * --------------------------------------------------------------------------*/
template<typename T>
class Fraccion {
    // ------------------------------------------------------------------------
    //  Restricción del parámetro de la plantilla   [¡AL REVÉS QUE Complex!]
    // ------------------------------------------------------------------------
    // Complex<T> exigía coma flotante. Una fracción, por definición, es un
    // cociente de ENTEROS, así que aquí exigimos:
    //   - std::is_integral<T>: T es un tipo entero (int, long, short, ...).
    //     Con double no tendría sentido el mcd ni la forma canónica.
    //   - std::is_signed<T>: T admite negativos. Con unsigned no podríamos
    //     representar -3/4. Esta condición también excluye bool (que es
    //     integral pero sin signo).
    // El operador && combina ambas condiciones; si alguna es false, la
    // compilación se detiene con el mensaje indicado.
    static_assert(std::is_integral<T>::value && std::is_signed<T>::value,
                  "Fraccion<T> requiere un tipo entero con signo (int, long, long long...)");

    // ------------------------------------------------------------------------
    //  Atributos privados (representación interna)
    // ------------------------------------------------------------------------
    // Privados por estar antes de cualquier etiqueta de acceso en una 'class'.
    // Siempre cumplen el invariante (I1), (I2), (I3) descrito arriba.
    T num_,   // numerador   (lleva el signo de la fracción)
      den_;   // denominador (siempre > 0)

    /* ------------------------------------------------------------------------
     *  [NUEVO] FUNCIÓN MIEMBRO PRIVADA: normalizar()
     *  -----------------------------------------------------------------------
     *  Es una función de AYUDA interna: está en la parte privada porque el
     *  usuario no debe (ni necesita) llamarla; la clase la usa para
     *  RESTABLECER EL INVARIANTE tras construir o modificar el objeto.
     *  No es 'const' porque, precisamente, modifica num_ y den_.
     *
     *  Pasos:
     *   1. Si el denominador es 0 -> el objeto no puede existir: se LANZA
     *      una excepción (ver constructor).
     *   2. Si el denominador es negativo -> se cambia el signo de ambos
     *      términos:  3/-4  ->  -3/4   (el valor no cambia: -a/-b = a/b).
     *   3. Se divide numerador y denominador por su mcd -> irreducible.
     *        std::gcd(a, b) devuelve el máximo común divisor de |a| y |b|
     *        (siempre >= 0, ignora el signo).
     *        Como den_ != 0, g es al menos 1: nunca dividimos por cero.
     *        Caso especial: gcd(0, d) = d, por lo que 0/5 -> 0/1. Así el
     *        cero también tiene una única representación.
     * ----------------------------------------------------------------------*/
    void normalizar()
    {
        if (den_ == 0)
            throw std::invalid_argument("Fraccion: el denominador no puede ser 0");

        if (den_ < 0) {        // (I2): el signo pasa al numerador
            num_ = -num_;
            den_ = -den_;
        }

        T const g = std::gcd(num_, den_);   // (I3): simplificación
        num_ /= g;
        den_ /= g;
    }

public:
    /* ------------------------------------------------------------------------
     *  CONSTRUCTOR
     *  -----------------------------------------------------------------------
     *  - Parámetros por referencia constante (T const&), como en Complex.
     *  - Valores por defecto:
     *        n = T{}   -> 0  (inicialización por valor de un entero)
     *        d = T{1}  -> 1  (¡NO puede ser T{}, sería 0 y violaría I1!)
     *    De modo que:
     *        Fraccion<int> a;        // 0/1
     *        Fraccion<int> b{5};     // 5/1  (un entero es una fracción)
     *        Fraccion<int> c{6, -8}; // -3/4 (normalizada automáticamente)
     *  - Lista de inicialización : num_{n}, den_{d}  copia los valores tal
     *    cual; el CUERPO del constructor llama a normalizar() para
     *    garantizar el invariante. A diferencia de Complex, aquí el cuerpo
     *    NO está vacío: tiene trabajo que hacer.
     *
     *  [NUEVO] EXCEPCIONES:
     *    Un constructor no tiene valor de retorno, así que no puede
     *    "devolver un código de error". Si recibe d == 0, normalizar()
     *    ejecuta 'throw', que interrumpe la construcción: el objeto NUNCA
     *    llega a existir, y el control salta al bloque 'catch' más cercano
     *    del código que intentaba crearlo (ver main). Si nadie captura la
     *    excepción, el programa termina con un mensaje de error.
     *
     *  - Constructor NO explícito: permite la conversión implícita de un
     *    entero T a Fraccion<T> (Fraccion<int> f = 3;  ->  3/1). Gracias a
     *    ella, más abajo podremos escribir f + 2 o 2 * f.
     * ----------------------------------------------------------------------*/
    Fraccion(T const& n = T{}, T const& d = T{1})
        : num_{n}, den_{d}
    {
        normalizar();   // establece el invariante (o lanza excepción)
    }

    /* ------------------------------------------------------------------------
     *  FUNCIONES DE ACCESO: SOLO LECTURA
     *  -----------------------------------------------------------------------
     *  En Complex había dos versiones de real():
     *      T const& real() const;   // lectura
     *      T&       real();         // escritura
     *  Aquí, DELIBERADAMENTE, solo existe la versión const. Si existiera
     *      T& den();
     *  el usuario podría escribir  f.den() = 0;  o  f.num() = 4;  (dejando
     *  4/4 sin simplificar) y romper el invariante desde fuera. La única
     *  forma de cambiar una fracción es asignarle otra o usar los
     *  operadores, que sí mantienen el invariante.
     * ----------------------------------------------------------------------*/
    T const& num() const { return num_; }
    T const& den() const { return den_; }

    /* ------------------------------------------------------------------------
     *  CONVERSIÓN A COMA FLOTANTE
     *  -----------------------------------------------------------------------
     *  static_cast<double>(num_) convierte el numerador a double ANTES de
     *  dividir. Si no lo hiciéramos, num_ / den_ sería una DIVISIÓN ENTERA
     *  (3/4 daría 0, no 0.75). Basta con convertir un operando: el otro se
     *  promociona a double automáticamente.
     * ----------------------------------------------------------------------*/
    double valor() const { return static_cast<double>(num_) / den_; }

    /* ------------------------------------------------------------------------
     *  INVERSA:  (n/d)^-1 = d/n
     *  -----------------------------------------------------------------------
     *  Función const: no modifica *this, devuelve una fracción NUEVA.
     *  La inversa de 0 no existe -> se lanza std::domain_error ("el valor
     *  está fuera del dominio de la operación").
     *  'return {den_, num_};' construye Fraccion(den_, num_) llamando al
     *  constructor, que normaliza el signo si num_ era negativo:
     *      (-3/4)^-1 -> Fraccion(4, -3) -> -4/3
     * ----------------------------------------------------------------------*/
    Fraccion inversa() const
    {
        if (num_ == 0)
            throw std::domain_error("Fraccion: el 0 no tiene inversa");
        return {den_, num_};
    }

    /* ------------------------------------------------------------------------
     *  [NUEVO] OPERADOR UNARIO MENOS:  -f
     *  -----------------------------------------------------------------------
     *  Tiene UN solo operando (el propio objeto), por eso no recibe
     *  parámetros. No modifica *this (es const) y devuelve una fracción
     *  nueva por valor. Cambiar el signo del numerador no rompe el
     *  invariante, pero se pasa igualmente por el constructor por sencillez.
     * ----------------------------------------------------------------------*/
    Fraccion operator-() const { return {-num_, den_}; }

    /* ========================================================================
     *  OPERADORES DE ASIGNACIÓN COMPUESTA  (+=, -=, *=, /=)
     *  -----------------------------------------------------------------------
     *  Igual que en Complex: funciones miembro que modifican *this y
     *  devuelven Fraccion& (return *this) para permitir encadenar.
     *
     *  DIFERENCIA con Complex: aquí hay UNA sola versión de cada operador
     *  (con Fraccion const&), no dos (Fraccion y T). ¿Cómo funciona entonces
     *  f += 2?  Al ser funciones miembro NO plantilla, el compilador puede
     *  aplicar la conversión implícita del constructor: 2 -> Fraccion(2, 1).
     *  Una versión específica para T sería algo más eficiente, pero
     *  duplicaría código; aquí priman la claridad y la corrección.
     *
     *  REGLA COMÚN: todos los resultados se calculan primero en variables
     *  LOCALES y solo al final se modifican num_ y den_. Así la
     *  auto-asignación (f += f, f *= f) funciona correctamente aunque
     *  'b' y '*this' sean el mismo objeto (mismo motivo que la variable 'r'
     *  en Complex::operator*=).
     * ======================================================================*/

    // ------------------------------- SUMA ---------------------------------
    // a/b + c/d usando el MÍNIMO COMÚN MÚLTIPLO de los denominadores en
    // lugar de b*d. Ejemplo: 1/6 + 1/4
    //     con b*d:          4/24 + 6/24 = 10/24 -> (simplificar) 5/12
    //     con mcm(6,4)=12:  2/12 + 3/12 = 5/12
    // Usar el mcm produce números intermedios más pequeños y reduce el
    // riesgo de desbordamiento.
    // l / den_ es una división EXACTA (l es múltiplo de den_).
    // Hay que normalizar al final: 1/2 + 1/2 = 2/2 -> 1/1.
    Fraccion& operator+=(Fraccion const& b)
    {
        T const l = std::lcm(den_, b.den_);                    // denominador común
        T const n = num_ * (l / den_) + b.num_ * (l / b.den_); // nuevo numerador
        num_ = n;
        den_ = l;
        normalizar();   // restablece el invariante
        return *this;
    }

    // ------------------------------- RESTA --------------------------------
    // a - b = a + (-b): se REUTILIZA la suma y el operador unario menos.
    // Escribir la lógica una sola vez evita errores (si la suma es correcta,
    // la resta también lo es).
    Fraccion& operator-=(Fraccion const& b)
    {
        return *this += -b;   // -b se calcula ANTES de modificar *this
    }

    // ------------------------------ PRODUCTO ------------------------------
    // a/b * c/d = (a*c)/(b*d), pero SIMPLIFICANDO EN CRUZ antes de
    // multiplicar: se divide 'a' y 'd' por su mcd, y 'c' y 'b' por el suyo.
    // Ejemplo: 4/9 * 3/8
    //     sin simplificar:  12/72 -> 1/6
    //     en cruz: g1 = mcd(4,8) = 4, g2 = mcd(3,9) = 3
    //              (4/4 * 3/3) / (9/3 * 8/4) = 1/6   (sin números grandes)
    // Como a/b y c/d ya eran irreducibles, el resultado TAMBIÉN lo es y el
    // denominador sigue siendo positivo, así que no hace falta normalizar().
    Fraccion& operator*=(Fraccion const& b)
    {
        T const g1 = std::gcd(num_, b.den_);
        T const g2 = std::gcd(b.num_, den_);
        T const n  = (num_ / g1) * (b.num_ / g2);
        T const d  = (den_ / g2) * (b.den_ / g1);
        num_ = n;
        den_ = d;
        return *this;
    }

    // ------------------------------ COCIENTE ------------------------------
    // a / b = a * b^-1. Se reutiliza el producto y la inversa; si b es 0,
    // b.inversa() lanza std::domain_error y *this queda SIN MODIFICAR
    // (la excepción salta antes de llegar a *=). Esto se llama "garantía
    // fuerte" frente a excepciones: o la operación se completa, o el objeto
    // queda exactamente como estaba.
    Fraccion& operator/=(Fraccion const& b)
    {
        return *this *= b.inversa();
    }

    /* ========================================================================
     *  [NUEVO] OPERADORES BINARIOS COMO FUNCIONES AMIGAS (friend)
     *  -----------------------------------------------------------------------
     *  En Complex<T>, los operadores binarios eran PLANTILLAS de función
     *  externas, y hacían falta TRES versiones de cada uno:
     *      Complex + Complex,   Complex + T,   T + Complex
     *  porque en una plantilla de función el compilador DEDUCE T a partir de
     *  los argumentos y NO aplica conversiones implícitas (por eso además
     *  'x + 2' fallaba con Complex<double>: el 2 es int, no double).
     *
     *  Aquí usamos otra técnica: definir el operador DENTRO de la clase con
     *  la palabra 'friend'. Consecuencias:
     *    1. NO es una función miembro: es una función libre normal, con dos
     *       parámetros, en la que el operando izquierdo NO tiene por qué ser
     *       una Fraccion. Por eso funciona  2 + f.
     *    2. NO es una plantilla de función: se genera una función concreta
     *       por cada Fraccion<T> instanciada. Al no haber deducción, el
     *       compilador SÍ aplica la conversión implícita int -> Fraccion.
     *       Resultado: UNA sola versión cubre f + g, f + 2 y 2 + f.
     *    3. Al ser 'friend', puede acceder a los miembros privados (num_,
     *       den_), aunque aquí basta con la interfaz pública.
     *    4. Solo la encuentra el compilador cuando alguno de los operandos
     *       es una Fraccion (búsqueda dependiente de argumentos, ADL); así no
     *       "contamina" el resto del programa. Por eso se las llama
     *       "amigas ocultas" (hidden friends).
     *
     *  Los parámetros:
     *    - 'Fraccion a' se recibe POR VALOR (es ya una copia), así que
     *      podemos modificarla directamente con += y devolverla. Equivale al
     *      'auto r = x;' de Complex, pero ahorrando una línea.
     *    - 'Fraccion const& b' por referencia constante, como siempre.
     *  Dentro de la clase, 'Fraccion' equivale a 'Fraccion<T>'.
     * ======================================================================*/

    friend Fraccion operator+(Fraccion a, Fraccion const& b) { return a += b; }
    friend Fraccion operator-(Fraccion a, Fraccion const& b) { return a -= b; }
    friend Fraccion operator*(Fraccion a, Fraccion const& b) { return a *= b; }
    friend Fraccion operator/(Fraccion a, Fraccion const& b) { return a /= b; }

    /* ========================================================================
     *  OPERADORES DE RELACIÓN
     *  -----------------------------------------------------------------------
     *  IGUALDAD: aquí se ve la RECOMPENSA del invariante. Como cada número
     *  racional tiene una única representación (forma canónica), dos
     *  fracciones son iguales si y solo si coinciden numerador y
     *  denominador. Sin forma canónica, 1/2 y 2/4 parecerían distintas y
     *  habría que comparar a*d == c*b.
     *
     *  Además, la comparación de fracciones es EXACTA, a diferencia de la
     *  comparación de double que se advertía en Complex:
     *      0.1 + 0.2 == 0.3                   -> false  (error de redondeo)
     *      1/10 + 2/10 == 3/10 (fracciones)   -> true
     *
     *  [NUEVO] ORDEN ( < ): los complejos NO tienen un orden natural
     *  (¿es (1,2) menor que (2,1)?), por eso Complex no definía '<'. Los
     *  racionales SÍ:  a/b < c/d  <=>  a*d < c*b
     *  Esta equivalencia (multiplicar en cruz) solo es válida porque b y d
     *  son POSITIVOS (invariante I2): multiplicar una desigualdad por un
     *  número negativo invertiría su sentido. Otra vez el invariante nos
     *  simplifica el código.
     *
     *  El resto ( != > <= >= ) se definen a partir de == y <, para no
     *  repetir lógica y garantizar que sean coherentes entre sí:
     *      a != b  <=>  !(a == b)
     *      a >  b  <=>  b < a
     *      a <= b  <=>  !(b < a)
     *      a >= b  <=>  !(a < b)
     * ======================================================================*/

    friend bool operator==(Fraccion const& a, Fraccion const& b)
    {
        return a.num_ == b.num_ && a.den_ == b.den_;
    }
    friend bool operator!=(Fraccion const& a, Fraccion const& b) { return !(a == b); }

    friend bool operator<(Fraccion const& a, Fraccion const& b)
    {
        return a.num_ * b.den_ < b.num_ * a.den_;
    }
    friend bool operator> (Fraccion const& a, Fraccion const& b) { return b < a; }
    friend bool operator<=(Fraccion const& a, Fraccion const& b) { return !(b < a); }
    friend bool operator>=(Fraccion const& a, Fraccion const& b) { return !(a < b); }

    /* ========================================================================
     *  [NUEVO] OPERADOR DE SALIDA  <<
     *  -----------------------------------------------------------------------
     *  Permite escribir  std::cout << f;  igual que con un int.
     *    - El operando IZQUIERDO es el flujo (std::ostream&), no una
     *      Fraccion, por lo que NO puede ser función miembro de Fraccion:
     *      debe ser una función libre (aquí, amiga).
     *    - El flujo se recibe y se devuelve POR REFERENCIA (no se puede
     *      copiar un flujo). Devolverlo permite encadenar:
     *          std::cout << f << " y " << g << '\n';
     *      que se evalúa como ((std::cout << f) << " y ") << g ...
     *    - Si el denominador es 1, se muestra solo el numerador (5 en vez
     *      de 5/1).
     * ======================================================================*/
    friend std::ostream& operator<<(std::ostream& os, Fraccion const& f)
    {
        os << f.num_;
        if (f.den_ != 1)
            os << '/' << f.den_;
        return os;
    }
};  // ';' obligatorio tras la definición de la clase


/* ############################################################################
 *  PARTE 2: PROGRAMA DE PRUEBA
 * ##########################################################################*/

int main()
{
    // ------------------------------------------------------------------------
    // 1. Construcción y forma canónica (invariante)
    // ------------------------------------------------------------------------
    std::cout << "--- 1. Construccion y forma canonica ---\n";
    Fraccion<int> a,            // 0/1  (valores por defecto)
                  b{5},         // 5/1  (un entero es una fraccion)
                  c{6, -8},     // -3/4 (signo al numerador y simplificada)
                  d{0, 7};      // 0/1  (el cero tiene una unica forma)
    std::cout << "a = " << a << ", b = " << b
              << ", c = " << c << ", d = " << d << '\n';
    std::cout << "c: num() = " << c.num() << ", den() = " << c.den()
              << ", valor() = " << c.valor() << '\n';

    // c.den() = 0;             // ERROR: assignment of read-only location
    //                          // (solo existe la version const de den())
    // c.num_ = 4;              // ERROR: 'num_' is a private member
    // Fraccion<double> x;      // ERROR: static_assert -> double no es entero
    // Fraccion<unsigned> y;    // ERROR: static_assert -> unsigned no tiene signo

    // ------------------------------------------------------------------------
    // 2. Excepciones: denominador cero
    // ------------------------------------------------------------------------
    // 'try' delimita el código que puede lanzar una excepción. Si se lanza,
    // el resto del bloque try se salta y se ejecuta el 'catch' cuyo tipo
    // coincida. 'e.what()' devuelve el mensaje pasado al lanzarla.
    // Se captura por referencia constante (const&) para no copiar el objeto
    // excepción.
    std::cout << "\n--- 2. Excepciones ---\n";
    try {
        Fraccion<int> mal{3, 0};                         // lanza invalid_argument
        std::cout << "Esto no se imprime: " << mal << '\n';
    } catch (std::invalid_argument const& e) {
        std::cout << "Capturada: " << e.what() << '\n';
    }

    // ------------------------------------------------------------------------
    // 3. Operadores aritmeticos (incluidos enteros a ambos lados)
    // ------------------------------------------------------------------------
    std::cout << "\n--- 3. Aritmetica ---\n";
    Fraccion<int> p{1, 6},
                  q{1, 4};
    std::cout << p << " + " << q << " = " << (p + q) << '\n';   // 5/12
    std::cout << p << " - " << q << " = " << (p - q) << '\n';   // -1/12
    std::cout << Fraccion<int>{4, 9} << " * " << Fraccion<int>{3, 8}
              << " = " << Fraccion<int>{4, 9} * Fraccion<int>{3, 8} << '\n'; // 1/6
    std::cout << p << " / " << q << " = " << (p / q) << '\n';   // 2/3
    std::cout << "-(" << c << ") = " << -c << '\n';            // 3/4

    // Con enteros a la izquierda y a la derecha gracias a las amigas ocultas
    // (con Complex<double>, 'x + 2' no compilaba):
    std::cout << "2 + " << p << " = " << (2 + p) << '\n';       // 13/6
    std::cout << q << " * 3 = " << (q * 3) << '\n';             // 3/4
    std::cout << "1 - " << q << " = " << (1 - q) << '\n';       // 3/4

    // Expresion compuesta: 1/2 + 1/3 * (2 - 1/4)  =  1/2 + 7/12  =  13/12
    auto r = Fraccion<int>{1, 2} + Fraccion<int>{1, 3} * (2 - q);
    std::cout << "1/2 + 1/3 * (2 - 1/4) = " << r << '\n';

    // ------------------------------------------------------------------------
    // 4. Asignacion compuesta, encadenamiento y auto-asignacion
    // ------------------------------------------------------------------------
    std::cout << "\n--- 4. Asignacion compuesta ---\n";
    Fraccion<int> x{1, 2},
                  y{1, 3};
    x += y;                       // 1/2 + 1/3 = 5/6
    std::cout << "x += y  -> x = " << x << '\n';
    x *= x;                       // auto-asignacion: (5/6)^2 = 25/36
    std::cout << "x *= x  -> x = " << x << '\n';
    x -= x;                       // auto-asignacion: 0
    std::cout << "x -= x  -> x = " << x << '\n';
    Fraccion<int> s{1, 2}, t{1, 4}, u{1, 4};
    s += t += u;                  // s += (t += u): t = 1/2, s = 1
    std::cout << "s += t += u -> s = " << s << ", t = " << t << '\n';

    // ------------------------------------------------------------------------
    // 5. Division por cero: garantia fuerte
    // ------------------------------------------------------------------------
    std::cout << "\n--- 5. Division por cero ---\n";
    Fraccion<int> z{7, 3};
    try {
        z /= Fraccion<int>{};             // dividir entre 0/1 -> lanza
    } catch (std::domain_error const& e) {
        std::cout << "Capturada: " << e.what() << '\n';
    }
    std::cout << "z sigue valiendo " << z << '\n';   // 7/3 (sin modificar)

    // ------------------------------------------------------------------------
    // 6. Comparaciones: exactitud frente a double
    // ------------------------------------------------------------------------
    std::cout << "\n--- 6. Comparaciones ---\n";
    std::cout << std::boolalpha;
    std::cout << "0.1 + 0.2 == 0.3          -> " << (0.1 + 0.2 == 0.3) << '\n'; // false
    std::cout << "1/10 + 2/10 == 3/10       -> "
              << (Fraccion<int>{1, 10} + Fraccion<int>{2, 10} == Fraccion<int>{3, 10})
              << '\n';                                                       // true
    std::cout << "1/2 == 2/4                -> "
              << (Fraccion<int>{1, 2} == Fraccion<int>{2, 4}) << '\n';      // true
    std::cout << "-3/4 < 2/3                -> " << (c < Fraccion<int>{2, 3}) << '\n'; // true
    std::cout << "5/12 >= 1/2               -> " << ((p + q) >= Fraccion<int>{1, 2}) << '\n'; // false
    std::cout << "b == 5                    -> " << (b == 5) << '\n';       // true

    // ------------------------------------------------------------------------
    // 7. Uso con la biblioteca estandar: ordenar un vector de fracciones
    // ------------------------------------------------------------------------
    // std::sort utiliza operator< para comparar elementos. Como lo hemos
    // definido, Fraccion<int> se puede ordenar igual que un int.
    std::cout << "\n--- 7. std::sort ---\n";
    std::vector<Fraccion<int>> v{{3, 4}, {-1, 2}, {2, 3}, {5, 1}, {1, 3}, {0, 9}};
    std::sort(v.begin(), v.end());
    std::cout << "Ordenadas:";
    for (auto const& f : v)       // recorre v sin copiar (referencia const)
        std::cout << ' ' << f;
    std::cout << '\n';            // -1/2 0 1/3 2/3 3/4 5

    // ------------------------------------------------------------------------
    // 8. Otro tipo entero: Fraccion<long long>
    // ------------------------------------------------------------------------
    // Suma de la serie 1/1 + 1/2 + ... + 1/30. Los denominadores crecen muy
    // rapido; con int se desbordarian, con long long caben.
    std::cout << "\n--- 8. Fraccion<long long> ---\n";
    Fraccion<long long> h;
    for (long long k = 1; k <= 30; ++k)
        h += Fraccion<long long>{1, k};
    std::cout << "H(30) = " << h << " ~ " << h.valor() << '\n';

    return 0;
}
