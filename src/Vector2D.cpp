/**
 * @file Vector2D.cpp
 * @brief 2D向量类实现
 * @author QtGame Team
 * @date 2024
 */

#include "Vector2D.h"

Vector2D Vector2D::operator+(const Vector2D& other) const {
    return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const {
    return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::operator*(double scalar) const {
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator/(double scalar) const {
    if (scalar != 0) {
        return Vector2D(x / scalar, y / scalar);
    }
    return Vector2D(0, 0);
}

Vector2D& Vector2D::operator+=(const Vector2D& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

double Vector2D::length() const {
    return std::sqrt(x * x + y * y);
}

double Vector2D::lengthSquared() const {
    return x * x + y * y;
}

Vector2D Vector2D::normalized() const {
    double len = length();
    if (len != 0) {
        return *this / len;
    }
    return Vector2D(0, 0);
}

double Vector2D::distanceTo(const Vector2D& other) const {
    Vector2D diff = *this - other;
    return diff.length();
} 