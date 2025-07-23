/**
 * @file Vector2D.h
 * @brief 2D vector class definition
 * @author Justin0828
 * @date 2025-07-23
 */

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

/**
 * @brief 2D vector class
 * 
 * Used to represent 2D vector quantities like position, velocity, acceleration, etc. in the game
 */
class Vector2D {
public:
    double x; ///< X coordinate
    double y; ///< Y coordinate

    /**
     * @brief Default constructor
     */
    Vector2D() : x(0), y(0) {}

    /**
     * @brief Constructor
     * @param x X coordinate
     * @param y Y coordinate
     */
    Vector2D(double x, double y) : x(x), y(y) {}

    /**
     * @brief Vector addition
     * @param other Another vector
     * @return Vector2D Addition result
     */
    Vector2D operator+(const Vector2D& other) const;

    /**
     * @brief Vector subtraction
     * @param other Another vector
     * @return Vector2D Subtraction result
     */
    Vector2D operator-(const Vector2D& other) const;

    /**
     * @brief Scalar multiplication
     * @param scalar Scalar value
     * @return Vector2D Scalar multiplication result
     */
    Vector2D operator*(double scalar) const;

    /**
     * @brief Scalar division
     * @param scalar Scalar value
     * @return Vector2D Scalar division result
     */
    Vector2D operator/(double scalar) const;

    /**
     * @brief Vector addition assignment
     * @param other Another vector
     * @return Vector2D& Self reference
     */
    Vector2D& operator+=(const Vector2D& other);

    /**
     * @brief Vector subtraction assignment
     * @param other Another vector
     * @return Vector2D& Self reference
     */
    Vector2D& operator-=(const Vector2D& other);

    /**
     * @brief Calculate vector length
     * @return double Vector length
     */
    double length() const;

    /**
     * @brief Calculate squared vector length
     * @return double Squared vector length
     */
    double lengthSquared() const;

    /**
     * @brief Normalize vector
     * @return Vector2D Normalized vector
     */
    Vector2D normalized() const;

    /**
     * @brief Calculate distance between two points
     * @param other Another point
     * @return double Distance
     */
    double distanceTo(const Vector2D& other) const;
};

#endif // VECTOR2D_H 