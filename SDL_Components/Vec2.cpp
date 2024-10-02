#include "Vec2.h"

#include <cmath>
#include <cassert>
using namespace std;
#define assertm(condition, msg) assert(((void)msg, condition))

template<Number Type>
Vec2<Type>::Vec2() : x(0), y(0) {}
template<Number Type>
Vec2<Type>::Vec2(Type _x) : x(_x), y(0) {}
template<Number Type>
Vec2<Type>::Vec2(Type _x, Type _y) : x(_x), y(_y) {}
template<Number Type>
Vec2<Type>::Vec2(const Vec2<Type>& vec) : x(vec.x), y(vec.y) {}

template<Number Type>
Vec2<Type>& Vec2<Type>::operator=(const Vec2<Type>& vec) {
    x = vec.x; y = vec.y;
    return *this;
}

template<Number T1>
template<Number T2>
Vec2<T1>& Vec2<T1>::operator+=(const Vec2<T2>& vec) {
    x += vec.x; y += vec.y;
    return *this;
}
template Vec2<int>& Vec2<int>::operator+=(const Vec2<int>& vec);
template Vec2<float>& Vec2<float>::operator+=(const Vec2<float>& vec);
template Vec2<int>& Vec2<int>::operator+=(const Vec2<float>& vec);
template Vec2<float>& Vec2<float>::operator+=(const Vec2<int>& vec);

template<Number T1>
template<Number T2>
Vec2<T1>& Vec2<T1>::operator-=(const Vec2<T2>& vec) {
    x -= vec.x; y -= vec.y;
    return *this;
}
template Vec2<int>& Vec2<int>::operator-=(const Vec2<int>& vec);
template Vec2<int>& Vec2<int>::operator-=(const Vec2<float>& vec);
template Vec2<float>& Vec2<float>::operator-=(const Vec2<float>& vec);
template Vec2<float>& Vec2<float>::operator-=(const Vec2<int>& vec);

template<Number T1>
template<Number T2>
Vec2<T1>& Vec2<T1>::operator*=(T2 a) {
    x *= a; y *= a;
    return *this;
}
template Vec2<int>& Vec2<int>::operator*=(int a);
template Vec2<int>& Vec2<int>::operator*=(float a);
template Vec2<float>& Vec2<float>::operator*=(int a);
template Vec2<float>& Vec2<float>::operator*=(float a);

template<Number T1>
template<Number T2>
Vec2<T1>& Vec2<T1>::operator/=(T2 a) {
    x /= a; y /= a;
    return *this;
}
template Vec2<int>& Vec2<int>::operator/=(int a);
template Vec2<int>& Vec2<int>::operator/=(float a);
template Vec2<float>& Vec2<float>::operator/=(int a);
template Vec2<float>& Vec2<float>::operator/=(float a);

template<Number T>
T Vec2<T>::magnitude() {
    return (T)sqrt(x * x + y * y);
}
template<Number T>
T Vec2<T>::magnitude_squared() {
    return x * x + y * y;
}
template<Number T>
Vec2<T>& Vec2<T>::normalize() {
    *this /= this->magnitude();
    return *this;
}
template<Number T>
Vec2<float> Vec2<T>::normalized() {
    return 1.f/this->magnitude() * *this;
}

template<Number T>
Vec2<T>& Vec2<T>::rotate90() {
    T oldX = x;
    x = -y; y = oldX;
    return *this;
}
template<Number T>
Vec2<T>& Vec2<T>::rotate180() {
    x = -x; y = -y;
    return *this;
}
template<Number T>
Vec2<T>& Vec2<T>::rotate270() {
    T oldX = x;
    x = y; y = -oldX;
    return *this;
}
template<Number T>
Vec2<T>& Vec2<T>::rotate(float a) {
    T oldX = x;
    x = cos(a) * x - sin(a) * y;
    y = sin(a) * oldX + cos(a) * y;
    return *this;
}
template<Number T1>
template<Number T2>
Vec2<T1>& Vec2<T1>::rotate_around(const Vec2<T2>& vec, float a) {
    *this -= vec;
    rotate(a);
    return *this += vec;
}
template Vec2<int>& Vec2<int>::rotate_around(const Vec2<int>&, float);
template Vec2<int>& Vec2<int>::rotate_around(const Vec2<float>&, float);
template Vec2<float>& Vec2<float>::rotate_around(const Vec2<int>&, float);
template Vec2<float>& Vec2<float>::rotate_around(const Vec2<float>&, float);

template<Number T>
Vec2<T>::operator Vec2<int>() {
    return {(int)x, (int)y};
}
template<Number T>
Vec2<T>::operator Vec2<float>() {
    return {(float)x, (float)y};
}

template<Number T>
Vec2<T>& Vec2<T>::map(float(*f)(float)) {
    x = f(x); y = f(y);
    return *this;
}

template<Number T1, Number T2>
Vec2<T1> operator+(const Vec2<T1>& vec1, const Vec2<T2>& vec2) {
    return {(T1)(vec1.x + vec2.x), (T1)(vec1.y + vec2.y)};
} 
template Vec2<int> operator+(const Vec2<int>&, const Vec2<int>&);
template Vec2<int> operator+(const Vec2<int>&, const Vec2<float>&);
template Vec2<float> operator+(const Vec2<float>&, const Vec2<int>&);
template Vec2<float> operator+(const Vec2<float>&, const Vec2<float>&);

template<Number T1, Number T2>
Vec2<T1> operator-(const Vec2<T1>& vec1, const Vec2<T2>& vec2) {
    return {(T1)(vec1.x - vec2.x), (T1)(vec1.y - vec2.y)};
} 
template Vec2<int> operator-(const Vec2<int>&, const Vec2<int>&);
template Vec2<int> operator-(const Vec2<int>&, const Vec2<float>&);
template Vec2<float> operator-(const Vec2<float>&, const Vec2<int>&);
template Vec2<float> operator-(const Vec2<float>&, const Vec2<float>&);

template<Number T1, Number T2>
T1 operator*(const Vec2<T1>& vec1, const Vec2<T2>& vec2) {
    return (T1)(vec1.x * vec2.x + vec1.y * vec2.y);
} 
template int operator*(const Vec2<int>&, const Vec2<int>&);
template int operator*(const Vec2<int>&, const Vec2<float>&);
template float operator*(const Vec2<float>&, const Vec2<int>&);
template float operator*(const Vec2<float>&, const Vec2<float>&);

template<Number T1, Number T2>
Vec2<T1> operator*(const Vec2<T1>& vec, T2 a) {
    return {(T1)(vec.x * a), (T1)(vec.y * a)};
} 
template Vec2<int> operator*(const Vec2<int>&, int);
template Vec2<int> operator*(const Vec2<int>&, float);
template Vec2<float> operator*(const Vec2<float>&, int);
template Vec2<float> operator*(const Vec2<float>&, float);

template<Number T1, Number T2>
Vec2<T1> operator*(T1 a, const Vec2<T2>& vec) {
    return {(T1)(vec.x * a), (T1)(vec.y * a)};
}
template Vec2<int> operator*(int, const Vec2<int>&);
template Vec2<int> operator*(int, const Vec2<float>&);
template Vec2<float> operator*(float, const Vec2<int>&);
template Vec2<float> operator*(float, const Vec2<float>&);

template<Number T1, Number T2>
Vec2<T2> operator/(const Vec2<T1>& vec, T2 a) {
    assertm(a != 0, "Can\'t divide by zero!");
    return {(T2)(vec.x / a), (T2)(vec.y / a)};
}
template Vec2<int> operator/(const Vec2<int>& vec, int);
template Vec2<float> operator/(const Vec2<int>& vec, float);
template Vec2<int> operator/(const Vec2<float>& vec, int);
template Vec2<float> operator/(const Vec2<float>& vec, float);

template<Number T1, Number T2>
Vec2<float> lerp(const Vec2<T1>& vec1, const Vec2<T2>& vec2, float t) {
    return t * vec1 + (1.f - t) * vec2;
}
template Vec2<float> lerp(const Vec2<int>&, const Vec2<int>&, float);
template Vec2<float> lerp(const Vec2<int>&, const Vec2<float>&, float);
template Vec2<float> lerp(const Vec2<float>&, const Vec2<int>&, float);
template Vec2<float> lerp(const Vec2<float>&, const Vec2<float>&, float);

template<Number T>
Vec2<float> map_vec(const Vec2<T>& vec, float(*f)(float)) {
    return {f(vec.x), f(vec.y)};
}
template Vec2<float> map_vec(const Vec2<int>&, float(*f)(float));
template Vec2<float> map_vec(const Vec2<float>&, float(*f)(float));

template struct Vec2<int>;
template struct Vec2<float>;

Vec2<int> vec_floor(const Vec2<float>& vec) {
    return {(int)floor(vec.x), (int)floor(vec.y)};
}
Vec2<int> vec_ceil(const Vec2<float>& vec) {
    return {(int)ceil(vec.x), (int)ceil(vec.y)};
}

Vec2<float> angle_to_vec(float a) {
    return {cos(a), sin(a)};
}
