/**
 * @file Player.cpp
 * @brief Player class implementation
 * @author Justin0828
 * @date 2025-07-23
 */

#include "Player.h"
#include "Weapon.h"
#include "Item.h"
#include <chrono>
#include <algorithm>

Player::Player(const Vector2D& startPos, const QColor& playerColor, bool isPlayerOne) 
    : m_position(startPos), m_velocity(0, 0), m_state(PlayerState::STANDING),
      m_color(playerColor), m_isPlayerOne(isPlayerOne), m_facingRight(true),
      m_hp(GameConfig::PLAYER_MAX_HP), m_isMovingLeft(false), m_isMovingRight(false),
      m_isCrouching(false), m_isGrounded(false), m_currentTerrain(TerrainType::GROUND),
      m_lastAttackTime(0), m_hasAdrenaline(false), m_adrenalineEndTime(0), m_lastAdrenalineHeal(0) {
    
    // Default fist weapon
    m_weapon = std::make_shared<FistWeapon>();
}

Player::~Player() {
}

void Player::update(double deltaTime) {
    updatePhysics(deltaTime);
    updateAdrenalineEffect(deltaTime);
    
    // Update weapon
    if (m_weapon) {
        m_weapon->update(deltaTime);
    }
    
    // Update state
    if (m_isCrouching) {
        m_state = PlayerState::CROUCHING;
    } else if (!m_isGrounded) {
        m_state = PlayerState::JUMPING;
    } else if (m_isMovingLeft || m_isMovingRight) {
        m_state = PlayerState::MOVING;
    } else {
        m_state = PlayerState::STANDING;
    }
}

void Player::moveLeft() {
    if (m_isCrouching) return; // Cannot move while crouching
    
    m_isMovingLeft = true;
    m_facingRight = false;
}

void Player::moveRight() {
    if (m_isCrouching) return; // Cannot move while crouching
    
    m_isMovingRight = true;
    m_facingRight = true;
}

void Player::stopMoving() {
    m_isMovingLeft = false;
    m_isMovingRight = false;
}

void Player::jump() {
    if (m_isCrouching) return; // Cannot jump while crouching
    if (!m_isGrounded) return; // Can only jump when on ground
    
    m_velocity.y = GameConfig::PLAYER_JUMP_SPEED;
    m_isGrounded = false;
}

void Player::crouch() {
    if (!m_isGrounded) return; // Can only crouch when on ground
    
    m_isCrouching = true;
    m_velocity.x = 0; // Stop horizontal movement when crouching
}

void Player::stopCrouching() {
    m_isCrouching = false;
}

void Player::attack() {
    if (!m_weapon) return;
    
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    if (currentTime - m_lastAttackTime < 100) return; // Prevent too frequent attacks
    
    m_lastAttackTime = currentTime;
    m_state = PlayerState::ATTACKING;
    
    // Weapon attack is handled in GameEngine
}

bool Player::pickupItem(std::shared_ptr<Item> item) {
    if (!item || !item->isValid()) return false;
    
    return item->onPickup(this);
}

void Player::takeDamage(int damage) {
    m_hp -= damage;
    m_hp = std::max(0, m_hp); // Ensure HP doesn't go below 0
}

void Player::heal(int healAmount) {
    m_hp += healAmount;
    m_hp = std::min(GameConfig::PLAYER_MAX_HP, m_hp); // Ensure HP doesn't exceed maximum
}

void Player::applyAdrenaline(int duration) {
    m_hasAdrenaline = true;
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    m_adrenalineEndTime = currentTime + duration;
    m_lastAdrenalineHeal = currentTime;
}

void Player::setTerrainType(TerrainType terrain) {
    m_currentTerrain = terrain;
}

bool Player::isGrounded() const {
    return m_isGrounded;
}

bool Player::isAlive() const {
    return m_hp > 0;
}

bool Player::isInvisible() const {
    // Invisible when crouching on grass
    return m_currentTerrain == TerrainType::GRASS && m_isCrouching;
}

void Player::updatePhysics(double deltaTime) {
    // Handle horizontal movement
    if (!m_isCrouching) {
        double moveSpeed = getCurrentMoveSpeed();
        
        if (m_isMovingLeft) {
            m_velocity.x = -moveSpeed;
        } else if (m_isMovingRight) {
            m_velocity.x = moveSpeed;
        } else {
            // Apply friction
            m_velocity.x *= GameConfig::FRICTION;
            if (std::abs(m_velocity.x) < 10) {
                m_velocity.x = 0;
            }
        }
    }
    
    // Apply gravity
    if (!m_isGrounded) {
        m_velocity.y += GameConfig::GRAVITY * deltaTime;
    }
    
    // Update position
    m_position += m_velocity * deltaTime;
    
    // Boundary check
    if (m_position.x < 0) {
        m_position.x = 0;
        m_velocity.x = 0;
    }
    if (m_position.x > GameConfig::WINDOW_WIDTH - getWidth()) {
        m_position.x = GameConfig::WINDOW_WIDTH - getWidth();
        m_velocity.x = 0;
    }
    
    // Ground check (simple implementation, actual collision detection in GameEngine)
    if (m_position.y >= GameConfig::GROUND_LEVEL - getHeight()) {
        m_position.y = GameConfig::GROUND_LEVEL - getHeight();
        m_velocity.y = 0;
        m_isGrounded = true;
    }
    // Note: don't set m_isGrounded = false here, as it would conflict with GameEngine's collision detection
    // GameEngine is responsible for setting the correct ground state
}

void Player::updateAdrenalineEffect(double deltaTime) {
    if (!m_hasAdrenaline) return;
    
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Check if adrenaline has expired
    if (currentTime >= m_adrenalineEndTime) {
        m_hasAdrenaline = false;
        return;
    }
    
    // Heal every second
    if (currentTime - m_lastAdrenalineHeal >= 1000) {
        heal(GameConfig::ADRENALINE_HEAL);
        m_lastAdrenalineHeal = currentTime;
    }
}

double Player::getCurrentMoveSpeed() const {
    double baseSpeed = GameConfig::PLAYER_SPEED;
    
    // Ice terrain speed boost
    if (m_currentTerrain == TerrainType::ICE) {
        baseSpeed *= GameConfig::ICE_SPEED_MULTIPLIER;
    }
    
    // Adrenaline speed boost
    if (m_hasAdrenaline) {
        baseSpeed *= GameConfig::ADRENALINE_SPEED_MULTIPLIER;
    }
    
    return baseSpeed;
} 