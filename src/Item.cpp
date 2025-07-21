/**
 * @file Item.cpp
 * @brief 物品系统类实现
 * @author QtGame Team
 * @date 2024
 */

#include "Item.h"
#include "Player.h"
#include <chrono>

// ======================== Item 基类实现 ========================

Item::Item(ItemType type, const Vector2D& position) 
    : m_type(type), m_position(position), m_velocity(0, 0), m_isGrounded(false), m_isValid(true) {
    m_spawnTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // 根据类型设置属性
    switch (type) {
        case ItemType::WEAPON_KNIFE:
            m_width = 25;
            m_height = 8;
            m_color = QColor(192, 192, 192); // 银色
            break;
        case ItemType::WEAPON_BALL:
            m_width = 16;
            m_height = 16;
            m_color = QColor(255, 165, 0); // 橙色
            break;
        case ItemType::WEAPON_RIFLE:
            m_width = 40;
            m_height = 12;
            m_color = QColor(128, 128, 128); // 灰色
            break;
        case ItemType::WEAPON_SNIPER:
            m_width = 50;
            m_height = 15;
            m_color = QColor(64, 64, 64); // 深灰色
            break;
        case ItemType::BANDAGE:
            m_width = 20;
            m_height = 15;
            m_color = QColor(255, 255, 255); // 白色
            break;
        case ItemType::MEDKIT:
            m_width = 25;
            m_height = 20;
            m_color = QColor(255, 0, 0); // 红色
            break;
        case ItemType::ADRENALINE:
            m_width = 15;
            m_height = 25;
            m_color = QColor(0, 255, 0); // 绿色
            break;
    }
    
    // 设置初始下降速度
    m_velocity.y = 200;
}

void Item::update(double deltaTime) {
    // 应用重力
    if (!m_isGrounded) {
        m_velocity.y += GameConfig::GRAVITY * deltaTime;
        m_position += m_velocity * deltaTime;
    }
}

bool Item::isValid() const {
    if (!m_isValid) {
        return false;
    }
    
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // 检查是否超出存在时间
    return (currentTime - m_spawnTime) < ITEM_LIFETIME;
}

// ======================== WeaponItem 类实现 ========================

WeaponItem::WeaponItem(ItemType type, const Vector2D& position) : Item(type, position) {
}

bool WeaponItem::onPickup(Player* player) {
    auto weapon = createWeapon();
    if (weapon) {
        // 玩家装备新武器（如果有旧武器会被替换）
        player->setWeapon(weapon);
        m_isValid = false;
        return true;
    }
    return false;
}

std::shared_ptr<Weapon> WeaponItem::createWeapon() {
    switch (m_type) {
        case ItemType::WEAPON_KNIFE:
            return std::make_shared<KnifeWeapon>();
        case ItemType::WEAPON_BALL:
            return std::make_shared<BallWeapon>();
        case ItemType::WEAPON_RIFLE:
            return std::make_shared<RifleWeapon>();
        case ItemType::WEAPON_SNIPER:
            return std::make_shared<SniperWeapon>();
        default:
            return nullptr;
    }
}

// ======================== BandageItem 类实现 ========================

BandageItem::BandageItem(const Vector2D& position) : Item(ItemType::BANDAGE, position) {
}

bool BandageItem::onPickup(Player* player) {
    if (player->getHP() < player->getMaxHP()) {
        player->heal(GameConfig::BANDAGE_HEAL);
        m_isValid = false;
        return true;
    }
    return false; // 满血时无法使用
}

// ======================== MedkitItem 类实现 ========================

MedkitItem::MedkitItem(const Vector2D& position) : Item(ItemType::MEDKIT, position) {
}

bool MedkitItem::onPickup(Player* player) {
    if (player->getHP() < player->getMaxHP()) {
        player->heal(GameConfig::MEDKIT_HEAL);
        m_isValid = false;
        return true;
    }
    return false; // 满血时无法使用
}

// ======================== AdrenalineItem 类实现 ========================

AdrenalineItem::AdrenalineItem(const Vector2D& position) : Item(ItemType::ADRENALINE, position) {
}

bool AdrenalineItem::onPickup(Player* player) {
    player->applyAdrenaline(GameConfig::ADRENALINE_DURATION);
    m_isValid = false;
    return true;
} 