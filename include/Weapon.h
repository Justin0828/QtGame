/**
 * @file Weapon.h
 * @brief 武器系统类定义
 * @author QtGame Team
 * @date 2024
 */

#ifndef WEAPON_H
#define WEAPON_H

#include "Vector2D.h"
#include "GameConfig.h"
#include <memory>
#include <QColor>

// 前向声明
class Player;

/**
 * @brief 武器类型枚举
 */
enum class WeaponType {
    FIST,       ///< 拳头
    KNIFE,      ///< 小刀
    BALL,       ///< 实心球
    RIFLE,      ///< 步枪
    SNIPER      ///< 狙击枪
};

/**
 * @brief 弹药类型枚举
 */
enum class AmmoType {
    MELEE,      ///< 近战攻击
    BULLET,     ///< 子弹
    THROWN      ///< 投掷物
};

/**
 * @brief 投射物类
 * 
 * 用于处理子弹、投掷物等可移动的攻击物体
 */
class Projectile {
public:
    /**
     * @brief 构造函数
     * @param startPos 起始位置
     * @param velocity 初始速度
     * @param damage 伤害值
     * @param type 弹药类型
     * @param ownerId 拥有者ID
     */
    Projectile(const Vector2D& startPos, const Vector2D& velocity, int damage, AmmoType type, int ownerId);

    /**
     * @brief 更新投射物状态
     * @param deltaTime 时间增量
     */
    void update(double deltaTime);

    /**
     * @brief 检查是否仍然有效
     * @return bool 是否有效
     */
    bool isValid() const;

    // Getter 方法
    Vector2D getPosition() const { return m_position; }
    Vector2D getVelocity() const { return m_velocity; }
    int getDamage() const { return m_damage; }
    AmmoType getType() const { return m_type; }
    int getOwnerId() const { return m_ownerId; }
    double getRadius() const { return m_radius; }

private:
    Vector2D m_position;    ///< 位置
    Vector2D m_velocity;    ///< 速度
    int m_damage;           ///< 伤害值
    AmmoType m_type;        ///< 弹药类型
    int m_ownerId;          ///< 拥有者ID
    double m_radius;        ///< 半径
    long long m_startTime;  ///< 创建时间
    static constexpr long long MAX_LIFETIME = 5000; ///< 最大存在时间（毫秒）
};

/**
 * @brief 武器基类
 */
class Weapon {
public:
    /**
     * @brief 构造函数
     * @param type 武器类型
     */
    explicit Weapon(WeaponType type);

    /**
     * @brief 虚析构函数
     */
    virtual ~Weapon() = default;

    /**
     * @brief 攻击方法
     * @param player 攻击者
     * @param targetPos 目标位置
     * @return std::shared_ptr<Projectile> 生成的投射物（如果有）
     */
    virtual std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) = 0;

    /**
     * @brief 检查是否可以攻击
     * @return bool 是否可以攻击
     */
    virtual bool canAttack() const;

    /**
     * @brief 更新武器状态
     * @param deltaTime 时间增量
     */
    virtual void update(double deltaTime);

    /**
     * @brief 获取攻击范围
     * @return double 攻击范围
     */
    virtual double getAttackRange() const = 0;

    // Getter 方法
    WeaponType getType() const { return m_type; }
    int getAmmo() const { return m_ammo; }
    int getDamage() const { return m_damage; }
    QColor getColor() const { return m_color; }
    bool hasAmmo() const { return m_ammo > 0 || m_ammo == -1; } // -1表示无限弹药

protected:
    /**
     * @brief 消耗弹药
     */
    void consumeAmmo();

    /**
     * @brief 重置攻击冷却
     */
    void resetCooldown();

protected:
    WeaponType m_type;        ///< 武器类型
    int m_ammo;               ///< 弹药数量（-1表示无限）
    int m_damage;             ///< 伤害值
    int m_cooldown;           ///< 攻击冷却时间（毫秒）
    long long m_lastAttackTime; ///< 上次攻击时间
    QColor m_color;           ///< 武器颜色
};

/**
 * @brief 拳头武器类
 */
class FistWeapon : public Weapon {
public:
    FistWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 50.0; }
};

/**
 * @brief 小刀武器类
 */
class KnifeWeapon : public Weapon {
public:
    KnifeWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 60.0; }
};

/**
 * @brief 实心球武器类
 */
class BallWeapon : public Weapon {
public:
    BallWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 400.0; }
};

/**
 * @brief 步枪武器类
 */
class RifleWeapon : public Weapon {
public:
    RifleWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 600.0; }
};

/**
 * @brief 狙击枪武器类
 */
class SniperWeapon : public Weapon {
public:
    SniperWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 800.0; }
};

#endif // WEAPON_H 