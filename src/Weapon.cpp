/**
 * @file Weapon.cpp
 * @brief 武器系统类实现
 * @author QtGame Team
 * @date 2024
 */

#include "Weapon.h"
#include "Player.h"
#include <chrono>
#include <cmath>

// ======================== Projectile 类实现 ========================

Projectile::Projectile(const Vector2D& startPos, const Vector2D& velocity, int damage, AmmoType type, int ownerId)
    : m_position(startPos), m_velocity(velocity), m_damage(damage), m_type(type), m_ownerId(ownerId) {
    m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // 根据类型设置半径
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
    // 更新位置
    m_position += m_velocity * deltaTime;
    
    // 如果是投掷物，应用重力
    if (m_type == AmmoType::THROWN) {
        m_velocity.y += GameConfig::GRAVITY * deltaTime;
    }
}

bool Projectile::isValid() const {
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // 检查是否超出存在时间
    if (currentTime - m_startTime > MAX_LIFETIME) {
        return false;
    }
    
    // 检查是否超出屏幕边界
    if (m_position.x < -100 || m_position.x > GameConfig::WINDOW_WIDTH + 100 ||
        m_position.y < -100 || m_position.y > GameConfig::WINDOW_HEIGHT + 100) {
        return false;
    }
    
    return true;
}

// ======================== Weapon 基类实现 ========================

Weapon::Weapon(WeaponType type) : m_type(type), m_lastAttackTime(0) {
    switch (type) {
        case WeaponType::FIST:
            m_ammo = -1; // 无限使用
            m_damage = GameConfig::FIST_DAMAGE;
            m_cooldown = GameConfig::FIST_COOLDOWN;
            m_color = QColor(139, 69, 19); // 棕色
            break;
        case WeaponType::KNIFE:
            m_ammo = -1; // 无限使用
            m_damage = GameConfig::KNIFE_DAMAGE;
            m_cooldown = GameConfig::KNIFE_COOLDOWN;
            m_color = QColor(192, 192, 192); // 银色
            break;
        case WeaponType::BALL:
            m_ammo = GameConfig::BALL_COUNT;
            m_damage = GameConfig::BALL_DAMAGE;
            m_cooldown = GameConfig::BALL_COOLDOWN;
            m_color = QColor(255, 165, 0); // 橙色
            break;
        case WeaponType::RIFLE:
            m_ammo = GameConfig::RIFLE_AMMO;
            m_damage = GameConfig::RIFLE_DAMAGE;
            m_cooldown = GameConfig::RIFLE_COOLDOWN;
            m_color = QColor(128, 128, 128); // 灰色
            break;
        case WeaponType::SNIPER:
            m_ammo = GameConfig::SNIPER_AMMO;
            m_damage = GameConfig::SNIPER_DAMAGE;
            m_cooldown = GameConfig::SNIPER_COOLDOWN;
            m_color = QColor(64, 64, 64); // 深灰色
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
    // 武器基类暂时不需要更新逻辑
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

// ======================== FistWeapon 类实现 ========================

FistWeapon::FistWeapon() : Weapon(WeaponType::FIST) {
}

std::shared_ptr<Projectile> FistWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    resetCooldown();
    // 拳头是近战武器，不产生投射物，伤害直接在游戏引擎中处理
    return nullptr;
}

// ======================== KnifeWeapon 类实现 ========================

KnifeWeapon::KnifeWeapon() : Weapon(WeaponType::KNIFE) {
}

std::shared_ptr<Projectile> KnifeWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    resetCooldown();
    // 小刀是近战武器，不产生投射物，伤害直接在游戏引擎中处理
    return nullptr;
}

// ======================== BallWeapon 类实现 ========================

BallWeapon::BallWeapon() : Weapon(WeaponType::BALL) {
}

std::shared_ptr<Projectile> BallWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    consumeAmmo();
    resetCooldown();
    
    Vector2D playerPos = player->getPosition();
    Vector2D startPos = playerPos + Vector2D(player->isFacingRight() ? 20 : -20, -10);
    
    // 计算抛物线投掷速度
    double direction = player->isFacingRight() ? 1.0 : -1.0;
    double angleRad = GameConfig::BALL_THROW_ANGLE * M_PI / 180.0;
    Vector2D velocity(
        GameConfig::BALL_THROW_SPEED * std::cos(angleRad) * direction,
        GameConfig::BALL_THROW_SPEED * std::sin(angleRad)
    );
    
    return std::make_shared<Projectile>(startPos, velocity, m_damage, AmmoType::THROWN, 
                                      player->getIsPlayerOne() ? 0 : 1);
}

// ======================== RifleWeapon 类实现 ========================

RifleWeapon::RifleWeapon() : Weapon(WeaponType::RIFLE) {
}

std::shared_ptr<Projectile> RifleWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    consumeAmmo();
    resetCooldown();
    
    Vector2D playerPos = player->getPosition();
    Vector2D startPos = playerPos + Vector2D(player->isFacingRight() ? 25 : -25, -15);
    
    // 水平射击
    Vector2D velocity(
        GameConfig::BULLET_SPEED * (player->isFacingRight() ? 1.0 : -1.0),
        0
    );
    
    return std::make_shared<Projectile>(startPos, velocity, m_damage, AmmoType::BULLET, 
                                      player->getIsPlayerOne() ? 0 : 1);
}

// ======================== SniperWeapon 类实现 ========================

SniperWeapon::SniperWeapon() : Weapon(WeaponType::SNIPER) {
}

std::shared_ptr<Projectile> SniperWeapon::attack(Player* player, const Vector2D& targetPos) {
    if (!canAttack()) {
        return nullptr;
    }
    
    consumeAmmo();
    resetCooldown();
    
    Vector2D playerPos = player->getPosition();
    Vector2D startPos = playerPos + Vector2D(player->isFacingRight() ? 30 : -30, -15);
    
    // 水平射击，速度更快
    Vector2D velocity(
        GameConfig::BULLET_SPEED * 1.5 * (player->isFacingRight() ? 1.0 : -1.0),
        0
    );
    
    return std::make_shared<Projectile>(startPos, velocity, m_damage, AmmoType::BULLET, 
                                      player->getIsPlayerOne() ? 0 : 1);
} 