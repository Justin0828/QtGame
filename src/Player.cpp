/**
 * @file Player.cpp
 * @brief 玩家类实现
 * @author QtGame Team
 * @date 2024
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
    
    // 默认装备拳头
    m_weapon = std::make_shared<FistWeapon>();
}

Player::~Player() {
}

void Player::update(double deltaTime) {
    updatePhysics(deltaTime);
    updateAdrenalineEffect(deltaTime);
    
    // 更新武器
    if (m_weapon) {
        m_weapon->update(deltaTime);
    }
    
    // 更新状态
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
    if (m_isCrouching) return; // 下蹲时无法移动
    
    m_isMovingLeft = true;
    m_facingRight = false;
}

void Player::moveRight() {
    if (m_isCrouching) return; // 下蹲时无法移动
    
    m_isMovingRight = true;
    m_facingRight = true;
}

void Player::stopMoving() {
    m_isMovingLeft = false;
    m_isMovingRight = false;
}

void Player::jump() {
    if (m_isCrouching) return; // 下蹲时无法跳跃
    if (!m_isGrounded) return; // 只能在地面时跳跃
    
    m_velocity.y = GameConfig::PLAYER_JUMP_SPEED;
    m_isGrounded = false;
}

void Player::crouch() {
    if (!m_isGrounded) return; // 只能在地面时下蹲
    
    m_isCrouching = true;
    m_velocity.x = 0; // 下蹲时停止水平移动
}

void Player::stopCrouching() {
    m_isCrouching = false;
}

void Player::attack() {
    if (!m_weapon) return;
    
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    if (currentTime - m_lastAttackTime < 100) return; // 防止攻击过于频繁
    
    m_lastAttackTime = currentTime;
    m_state = PlayerState::ATTACKING;
    
    // 武器攻击在GameEngine中处理
}

bool Player::pickupItem(std::shared_ptr<Item> item) {
    if (!item || !item->isValid()) return false;
    
    return item->onPickup(this);
}

void Player::takeDamage(int damage) {
    m_hp -= damage;
    m_hp = std::max(0, m_hp); // 确保血量不会小于0
}

void Player::heal(int healAmount) {
    m_hp += healAmount;
    m_hp = std::min(GameConfig::PLAYER_MAX_HP, m_hp); // 确保血量不会超过最大值
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
    // 在草地上下蹲时隐身
    return m_currentTerrain == TerrainType::GRASS && m_isCrouching;
}

void Player::updatePhysics(double deltaTime) {
    // 处理水平移动
    if (!m_isCrouching) {
        double moveSpeed = getCurrentMoveSpeed();
        
        if (m_isMovingLeft) {
            m_velocity.x = -moveSpeed;
        } else if (m_isMovingRight) {
            m_velocity.x = moveSpeed;
        } else {
            // 应用摩擦力
            m_velocity.x *= GameConfig::FRICTION;
            if (std::abs(m_velocity.x) < 10) {
                m_velocity.x = 0;
            }
        }
    }
    
    // 应用重力
    if (!m_isGrounded) {
        m_velocity.y += GameConfig::GRAVITY * deltaTime;
    }
    
    // 更新位置
    m_position += m_velocity * deltaTime;
    
    // 边界检查
    if (m_position.x < 0) {
        m_position.x = 0;
        m_velocity.x = 0;
    }
    if (m_position.x > GameConfig::WINDOW_WIDTH - getWidth()) {
        m_position.x = GameConfig::WINDOW_WIDTH - getWidth();
        m_velocity.x = 0;
    }
    
    // 地面检查（简单实现，实际碰撞检测在GameEngine中）
    if (m_position.y >= GameConfig::GROUND_LEVEL - getHeight()) {
        m_position.y = GameConfig::GROUND_LEVEL - getHeight();
        m_velocity.y = 0;
        m_isGrounded = true;
    }
    // 注意：不在这里设置m_isGrounded = false，因为这会与GameEngine的碰撞检测冲突
    // GameEngine会负责设置正确的地面状态
}

void Player::updateAdrenalineEffect(double deltaTime) {
    if (!m_hasAdrenaline) return;
    
    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // 检查肾上腺素是否过期
    if (currentTime >= m_adrenalineEndTime) {
        m_hasAdrenaline = false;
        return;
    }
    
    // 每秒回血
    if (currentTime - m_lastAdrenalineHeal >= 1000) {
        heal(GameConfig::ADRENALINE_HEAL);
        m_lastAdrenalineHeal = currentTime;
    }
}

double Player::getCurrentMoveSpeed() const {
    double baseSpeed = GameConfig::PLAYER_SPEED;
    
    // 冰面加速
    if (m_currentTerrain == TerrainType::ICE) {
        baseSpeed *= GameConfig::ICE_SPEED_MULTIPLIER;
    }
    
    // 肾上腺素加速
    if (m_hasAdrenaline) {
        baseSpeed *= GameConfig::ADRENALINE_SPEED_MULTIPLIER;
    }
    
    return baseSpeed;
} 