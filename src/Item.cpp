/**
 * @file Item.cpp
 * @brief Item system class implementations
 * @author Justin0828
 * @date 2025-07-23
 */

#include "Item.h"
#include "Player.h"
#include <chrono>

// ======================== Item base class implementation ========================

Item::Item(ItemType type, const Vector2D& position) 
    : m_type(type), m_position(position), m_velocity(0, 0), m_isGrounded(false), m_isValid(true) {
    m_spawnTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Set properties based on type
    switch (type) {
        case ItemType::WEAPON_KNIFE:
            m_width = 25;
            m_height = 8;
            m_color = QColor(192, 192, 192); // Silver
            break;
        case ItemType::WEAPON_BALL:
            m_width = 16;
            m_height = 16;
            m_color = QColor(255, 165, 0); // Orange
            break;
        case ItemType::WEAPON_RIFLE:
            m_width = 40;
            m_height = 12;
            m_color = QColor(128, 128, 128); // Gray
            break;
        case ItemType::WEAPON_SNIPER:
            m_width = 50;
            m_height = 15;
            m_color = QColor(64, 64, 64); // Dark gray
            break;
        case ItemType::BANDAGE:
            m_width = 20;
            m_height = 15;
            m_color = QColor(255, 255, 255); // White
            break;
        case ItemType::MEDKIT:
            m_width = 25;
            m_height = 20;
            m_color = QColor(255, 0, 0); // Red
            break;
        case ItemType::ADRENALINE:
            m_width = 15;
            m_height = 25;
            m_color = QColor(0, 255, 0); // Green
            break;
    }
    
    // Set initial falling velocity
    m_velocity.y = 200;
}

void Item::update(double deltaTime) {
    // Apply gravity
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
    
    // Check if exceeded lifetime
    return (currentTime - m_spawnTime) < ITEM_LIFETIME;
}

// ======================== WeaponItem class implementation ========================

WeaponItem::WeaponItem(ItemType type, const Vector2D& position) : Item(type, position) {
}

bool WeaponItem::onPickup(Player* player) {
    auto weapon = createWeapon();
    if (weapon) {
        // Player equips new weapon (old weapon will be replaced if exists)
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

// ======================== BandageItem class implementation ========================

BandageItem::BandageItem(const Vector2D& position) : Item(ItemType::BANDAGE, position) {
}

bool BandageItem::onPickup(Player* player) {
    if (player->getHP() < player->getMaxHP()) {
        player->heal(GameConfig::BANDAGE_HEAL);
        m_isValid = false;
        return true;
    }
    return false; // Cannot use when at full health
}

// ======================== MedkitItem class implementation ========================

MedkitItem::MedkitItem(const Vector2D& position) : Item(ItemType::MEDKIT, position) {
}

bool MedkitItem::onPickup(Player* player) {
    if (player->getHP() < player->getMaxHP()) {
        player->heal(GameConfig::MEDKIT_HEAL);
        m_isValid = false;
        return true;
    }
    return false; // Cannot use when at full health
}

// ======================== AdrenalineItem class implementation ========================

AdrenalineItem::AdrenalineItem(const Vector2D& position) : Item(ItemType::ADRENALINE, position) {
}

bool AdrenalineItem::onPickup(Player* player) {
    player->applyAdrenaline(GameConfig::ADRENALINE_DURATION);
    m_isValid = false;
    return true;
} 