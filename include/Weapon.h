/**
 * @file Weapon.h
 * @brief Weapon system class definitions
 * @author Justin0828
 * @date 2025-07-23
 */

#ifndef WEAPON_H
#define WEAPON_H

#include "Vector2D.h"
#include "GameConfig.h"
#include <memory>
#include <QColor>

// Forward declaration
class Player;

/**
 * @brief Weapon type enumeration
 */
enum class WeaponType {
    FIST,       ///< Fist
    KNIFE,      ///< Knife
    BALL,       ///< Ball
    RIFLE,      ///< Rifle
    SNIPER      ///< Sniper rifle
};

/**
 * @brief Ammunition type enumeration
 */
enum class AmmoType {
    MELEE,      ///< Melee attack
    BULLET,     ///< Bullet
    THROWN      ///< Thrown projectile
};

/**
 * @brief Projectile class
 * 
 * Handles bullets, thrown objects and other moving attack projectiles
 */
class Projectile {
public:
    /**
     * @brief Constructor
     * @param startPos Starting position
     * @param velocity Initial velocity
     * @param damage Damage value
     * @param type Ammunition type
     * @param ownerId Owner ID
     */
    Projectile(const Vector2D& startPos, const Vector2D& velocity, int damage, AmmoType type, int ownerId);

    /**
     * @brief Update projectile state
     * @param deltaTime Time delta
     */
    void update(double deltaTime);

    /**
     * @brief Check if still valid
     * @return bool Whether valid
     */
    bool isValid() const;

    // Getter methods
    Vector2D getPosition() const { return m_position; }
    Vector2D getVelocity() const { return m_velocity; }
    int getDamage() const { return m_damage; }
    AmmoType getType() const { return m_type; }
    int getOwnerId() const { return m_ownerId; }
    double getRadius() const { return m_radius; }

private:
    Vector2D m_position;    ///< Position
    Vector2D m_velocity;    ///< Velocity
    int m_damage;           ///< Damage value
    AmmoType m_type;        ///< Ammunition type
    int m_ownerId;          ///< Owner ID
    double m_radius;        ///< Radius
    long long m_startTime;  ///< Creation time
    static constexpr long long MAX_LIFETIME = 5000; ///< Maximum lifetime in milliseconds
};

/**
 * @brief Weapon base class
 */
class Weapon {
public:
    /**
     * @brief Constructor
     * @param type Weapon type
     */
    explicit Weapon(WeaponType type);

    /**
     * @brief Virtual destructor
     */
    virtual ~Weapon() = default;

    /**
     * @brief Attack method
     * @param player The attacker
     * @param targetPos Target position
     * @return std::shared_ptr<Projectile> Generated projectile (if any)
     */
    virtual std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) = 0;

    /**
     * @brief Check if can attack
     * @return bool Whether can attack
     */
    virtual bool canAttack() const;

    /**
     * @brief Update weapon state
     * @param deltaTime Time delta
     */
    virtual void update(double deltaTime);

    /**
     * @brief Get attack range
     * @return double Attack range
     */
    virtual double getAttackRange() const = 0;

    // Getter methods
    WeaponType getType() const { return m_type; }
    int getAmmo() const { return m_ammo; }
    int getDamage() const { return m_damage; }
    QColor getColor() const { return m_color; }
    bool hasAmmo() const { return m_ammo > 0 || m_ammo == -1; } // -1 means infinite ammo

protected:
    /**
     * @brief Consume ammunition
     */
    void consumeAmmo();

    /**
     * @brief Reset attack cooldown
     */
    void resetCooldown();

protected:
    WeaponType m_type;        ///< Weapon type
    int m_ammo;               ///< Ammunition count (-1 means infinite)
    int m_damage;             ///< Damage value
    int m_cooldown;           ///< Attack cooldown time in milliseconds
    long long m_lastAttackTime; ///< Last attack time
    QColor m_color;           ///< Weapon color
};

/**
 * @brief Fist weapon class
 */
class FistWeapon : public Weapon {
public:
    FistWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 50.0; }
};

/**
 * @brief Knife weapon class
 */
class KnifeWeapon : public Weapon {
public:
    KnifeWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 60.0; }
};

/**
 * @brief Ball weapon class
 */
class BallWeapon : public Weapon {
public:
    BallWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 400.0; }
};

/**
 * @brief Rifle weapon class
 */
class RifleWeapon : public Weapon {
public:
    RifleWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 600.0; }
};

/**
 * @brief Sniper rifle weapon class
 */
class SniperWeapon : public Weapon {
public:
    SniperWeapon();
    std::shared_ptr<Projectile> attack(Player* player, const Vector2D& targetPos) override;
    double getAttackRange() const override { return 800.0; }
};

#endif // WEAPON_H 