#pragma once

#include <concepts>

template <class Type>
concept Number = std::integral<Type> || std::floating_point<Type>;

template <Number Type>
struct Vec2 {
    Type x, y;

    Vec2();
    Vec2(Type _x);
    Vec2(Type _x, Type _y);
    Vec2(const Vec2<Type>& vec);

    Vec2<Type>& operator=(const Vec2<Type>& vec);
    template<Number T> Vec2<Type>& operator+=(const Vec2<T>& vec);
    template<Number T> Vec2<Type>& operator-=(const Vec2<T>& vec);
    template<Number T> Vec2<Type>& operator*=(T a);
    template<Number T> Vec2<Type>& operator/=(T a);

    Type magnitude();
    Type magnitude_squared();
    Vec2<Type>& normalize();
    Vec2<float> normalized();

    Vec2<Type>& rotate90();
    Vec2<Type>& rotate180();
    Vec2<Type>& rotate270();
    Vec2<Type>& rotate(float a);
    template<Number T> Vec2<Type>& rotate_around(const Vec2<T>& vec, float a);
    operator Vec2<int>();
    operator Vec2<float>();
};

template<Number T1, Number T2>
Vec2<T1> operator+(const Vec2<T1>& vec1, const Vec2<T2>& vec2);
template<Number T1, Number T2>
Vec2<T1> operator-(const Vec2<T1>& vec1, const Vec2<T2>& vec2);
template<Number T1, Number T2>
T1 operator*(const Vec2<T1>& vec1, const Vec2<T2>& vec2);
template<Number T1, Number T2>
Vec2<T1> operator*(const Vec2<T1>& vec, T2 a);
template<Number T1, Number T2>
Vec2<T1> operator*(T1 a, const Vec2<T2>& vec);
template<Number T1, Number T2>
Vec2<T2> operator/(const Vec2<T1>& vec, T2 a);

Vec2<int> vec_floor(const Vec2<float>& vec);
Vec2<int> vec_ceil(const Vec2<float>& vec);

Vec2<float> angle_to_vec(float a);
