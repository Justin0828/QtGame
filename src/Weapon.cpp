/**
 * @file Weapon.cpp
 * @brief Weapon system class implementations
 * @author Justin0828
 * @date 2025-07-23
 */

#include "Weapon.h"
#include "Player.h"
#include <chrono>
#include <cmath>

// ======================== Projectile class implementation ========================

Projectile::Projectile(const Vector2D& startPos, const Vector2D& velocity, int damage, AmmoType type, int ownerId)
    : m_position(startPos), m_velocity(velocity), m_damage(damage), m_type(type), m_ownerId(ownerId) {
    m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Set radius based on type
    switch (type) {
        case AmmoType::BULLET:
            m_radius = 3.0;
            break;
        case AmmoType::THROWN:
            m_radius = 8.0;
            break;
        default:
            m_radius = 5.0;
            break;
    }
}

void Projectile::update(double deltaTime) {
    // Update position
    m_position += m_velocity * deltaTime;
    
    // Apply gravity for thrown projectiles
    if (m_type == AmmoType::THROWN) {
        m_velocity.y += GameConfig::GRAVITY * deltaTime;
    }
}

bool Projectile::isValid() const {
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Check if exceeded lifetime
    if (currentTime - m_startTime > MAX_LIFETIME) {
        return false;
    }
    
    // Check if out of screen bounds
    if (m_position.x < -100 || m_position.x > GameConfig::WINDOW_WIDTH + 100 ||
        m_position.y < -100 || m_position.y > GameConfig::WINDOW_HEIGHT + 100) {
        return false;
    }
    
    return true;
}

// ======================== Weapon base class implementation ========================

Weapon::Weapon(WeaponType type) : m_type(type), m_lastAttackTime(0) {
    switch (type) {
        case WeaponType::FIST:
            m_ammo = -1; // Infinite use
            m_damage = GameConfig::FIST_DAMAGE;
            m_cooldown = GameConfig::FIST_COOLDOWN;
            m_color = QColor(139, 69, 19); // Brown
            break;
        case WeaponType::KNIFE:
            m_ammo = -1; // Infinite use
            m_damage = GameConfig::KNIFE_DAMAGE;
            m_cooldown = GameConfig::KNIFE_COOLDOWN;
            m_color = QColor(192, 192, 192); // Silver
            break;
        case WeaponType::BALL:
            m_ammo = GameConfig::BALL_COUNT;
            m_damage = GameConfig::BALL_DAMAGE;
            m_cooldown = GameConfig::BALL_COOLDOWN;
            m_color = QColor(255, 165, 0); // Orange
            break;
        case WeaponType::RIFLE:
            m_ammo = GameConfig::RIFLE_AMMO;
            m_damage = GameConfig::RIFLE_DAMAGE;
            m_cooldown = GameConfig::RIFLE_COOLDOWN;
            m_color = QColor(128, 128, 128); // Gray
            break;
        case WeaponType::SNIPER:
            m_ammo = GameConfig::SNIPER_AMMO;
            m_damage = GameConfig::SNIPER_DAMAGE;
            m_cooldown = GameConfig::SNIPER_COOLDOWN;
            m_color = QColor(64, 64, 64); // Dark gray
            break;
    }
}

bool Weapon::canAttack() const {
    if (!hasAmmo()) {
        return false;
    }
    
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    return (currentTime - m_lastAttackTime) >= m_cooldown;
}

void Weapon::update(double deltaTime) {
    // Weapon base class doesn't need update logic for now
}

void Weapon::consumeAmmo() {
    if (m_ammo > 0) {
        m_ammo--;
    }
}

void Weapon::resetCooldown() {
    m_lastAttackTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

// ======================== FistWeapon class implementation ========================

FistWeapon::FistWeapon() : Weapon(WeaponType::FIST) {
}

std::shared_ptr<Projectile> FistWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    resetCooldown();
    // Fist is melee weapon, no projectile generated, damage handled directly in game engine
    return nullptr;
}

// ======================== KnifeWeapon class implementation ========================

KnifeWeapon::KnifeWeapon() : Weapon(WeaponType::KNIFE) {
}

std::shared_ptr<Projectile> KnifeWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    resetCooldown();
    // Knife is melee weapon, no projectile generated, damage handled directly in game engine
    return nullptr;
}

// ======================== BallWeapon class implementation ========================

BallWeapon::BallWeapon() : Weapon(WeaponType::BALL) {
}

std::shared_ptr<Projectile> BallWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    consumeAmmo();
    resetCooldown();
    
    Vector2D playerPos = player->getPosition();
    // Fire ball from player center
    Vector2D startPos = playerPos + Vector2D(
        player->isFacingRight() ? player->getWidth() * 0.8 : player->getWidth() * 0.2 - 20, 
        player->getHeight() * 0.3  // Fire from player's upper body
    );
    
    // Calculate parabolic throw velocity
    double direction = player->isFacingRight() ? 1.0 : -1.0;
    double angleRad = GameConfig::BALL_THROW_ANGLE * M_PI / 180.0;
    Vector2D velocity(
        GameConfig::BALL_THROW_SPEED * std::cos(angleRad) * direction,
        GameConfig::BALL_THROW_SPEED * std::sin(angleRad)
    );
    
    return std::make_shared<Projectile>(startPos, velocity, m_damage, AmmoType::THROWN, 
                                      player->getIsPlayerOne() ? 0 : 1);
}

// ======================== RifleWeapon class implementation ========================

RifleWeapon::RifleWeapon() : Weapon(WeaponType::RIFLE) {
}

std::shared_ptr<Projectile> RifleWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    consumeAmmo();
    resetCooldown();
    
    Vector2D playerPos = player->getPosition();
    // Fire from player center height, horizontal offset based on facing direction
    Vector2D startPos = playerPos + Vector2D(
        player->isFacingRight() ? player->getWidth() : -5, 
        player->getHeight() * 0.4  // Fire from slightly above player center
    );
    
    // Horizontal shooting
    Vector2D velocity(
        GameConfig::BULLET_SPEED * (player->isFacingRight() ? 1.0 : -1.0),
        0
    );
    
    return std::make_shared<Projectile>(startPos, velocity, m_damage, AmmoType::BULLET, 
                                      player->getIsPlayerOne() ? 0 : 1);
}

// ======================== SniperWeapon class implementation ========================

SniperWeapon::SniperWeapon() : Weapon(WeaponType::SNIPER) {
}

std::shared_ptr<Projectile> SniperWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    consumeAmmo();
    resetCooldown();
    
    Vector2D playerPos = player->getPosition();
    // Fire from player center height, horizontal offset based on facing direction
    Vector2D startPos = playerPos + Vector2D(
        player->isFacingRight() ? player->getWidth() : -5, 
        player->getHeight() * 0.4  // Fire from slightly above player center
    );
    
    // Horizontal shooting, faster speed
    Vector2D velocity(
        GameConfig::BULLET_SPEED * 1.5 * (player->isFacingRight() ? 1.0 : -1.0),
        0
    );
    
    return std::make_shared<Projectile>(startPos, velocity, m_damage, AmmoType::BULLET, 
                                      player->getIsPlayerOne() ? 0 : 1);
} 