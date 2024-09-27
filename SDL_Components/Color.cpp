#include "Color.h"

Color::Color() : r(0), g(0), b(0), a(0) {}
Color::Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) : r(_r), g(_g), b(_b), a(_a) {}
Color& Color::operator*=(float x) {
    x = (x > 1 ? 1 : (x < 0 ? 0 : x));
    r *= x;
    g *= x;
    b *= x;
    return *this;
}
