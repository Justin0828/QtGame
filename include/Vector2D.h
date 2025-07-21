/**
 * @file Vector2D.h
 * @brief 2D向量类定义
 * @author QtGame Team
 * @date 2024
 */

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

/**
 * @brief 2D向量类
 * 
 * 用于表示游戏中的位置、速度、加速度等2D向量量
 */
class Vector2D {
public:
    double x; ///< X坐标
    double y; ///< Y坐标

    /**
     * @brief 默认构造函数
     */
    Vector2D() : x(0), y(0) {}

    /**
     * @brief 构造函数
     * @param x X坐标
     * @param y Y坐标
     */
    Vector2D(double x, double y) : x(x), y(y) {}

    /**
     * @brief 向量加法
     * @param other 另一个向量
     * @return Vector2D 相加结果
     */
    Vector2D operator+(const Vector2D& other) const;

    /**
     * @brief 向量减法
     * @param other 另一个向量
     * @return Vector2D 相减结果
     */
    Vector2D operator-(const Vector2D& other) const;

    /**
     * @brief 向量数乘
     * @param scalar 标量
     * @return Vector2D 数乘结果
     */
    Vector2D operator*(double scalar) const;

    /**
     * @brief 向量数除
     * @param scalar 标量
     * @return Vector2D 数除结果
     */
    Vector2D operator/(double scalar) const;

    /**
     * @brief 向量赋值加法
     * @param other 另一个向量
     * @return Vector2D& 自身引用
     */
    Vector2D& operator+=(const Vector2D& other);

    /**
     * @brief 向量赋值减法
     * @param other 另一个向量
     * @return Vector2D& 自身引用
     */
    Vector2D& operator-=(const Vector2D& other);

    /**
     * @brief 计算向量长度
     * @return double 向量长度
     */
    double length() const;

    /**
     * @brief 计算向量长度的平方
     * @return double 向量长度的平方
     */
    double lengthSquared() const;

    /**
     * @brief 标准化向量
     * @return Vector2D 标准化后的向量
     */
    Vector2D normalized() const;

    /**
     * @brief 计算两点间距离
     * @param other 另一个点
     * @return double 距离
     */
    double distanceTo(const Vector2D& other) const;
};

#endif // VECTOR2D_H 