/**
 * @file Player.h
 * @brief Player class definition
 * @author Justin0828
 * @date 2025-07-23
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "Vector2D.h"
#include "GameConfig.h"
#include <memory>
#include <QColor>

// Forward declarations
class Weapon;
class Item;

/**
 * @brief Player state enumeration
 */
enum class PlayerState {
    STANDING,   ///< Standing state
    MOVING,     ///< Moving state
    JUMPING,    ///< Jumping state
    CROUCHING,  ///< Crouching state
    ATTACKING   ///< Attacking state
};

/**
 * @brief Terrain type enumeration
 */
enum class TerrainType {
    GROUND,     ///< Normal ground
    GRASS,      ///< Grass terrain (stealth effect)
    ICE         ///< Ice terrain (speed boost effect)
};

/**
 * @brief Player class
 * 
 * Handles all player functionality including movement, combat, health, etc.
 */
class Player {
public:
    /**
     * @brief Constructor
     * @param startPos Initial position
     * @param playerColor Player color
     * @param isPlayerOne Whether this is player one
     */
    Player(const Vector2D& startPos, const QColor& playerColor, bool isPlayerOne);

    /**
     * @brief Destructor
     */
    ~Player();

    /**
     * @brief Update player state
     * @param deltaTime Time delta in seconds
     */
    void update(double deltaTime);

    /**
     * @brief Move left
     */
    void moveLeft();

    /**
     * @brief Move right
     */
    void moveRight();

    /**
     * @brief Stop moving
     */
    void stopMoving();

    /**
     * @brief Jump
     */
    void jump();

    /**
     * @brief Crouch
     */
    void crouch();

    /**
     * @brief Stop crouching
     */
    void stopCrouching();

    /**
     * @brief Attack
     */
    void attack();

    /**
     * @brief Pick up an item
     * @param item The item to pick up
     * @return bool Whether pickup was successful
     */
    bool pickupItem(std::shared_ptr<Item> item);

    /**
     * @brief Take damage
     * @param damage Damage amount
     */
    void takeDamage(int damage);

    /**
     * @brief Heal
     * @param healAmount Amount to heal
     */
    void heal(int healAmount);

    /**
     * @brief Apply adrenaline effect
     * @param duration Duration in milliseconds
     */
    void applyAdrenaline(int duration);

    /**
     * @brief Set terrain type
     * @param terrain Terrain type
     */
    void setTerrainType(TerrainType terrain);

    /**
     * @brief Check if on ground
     * @return bool Whether on ground
     */
    bool isGrounded() const;

    /**
     * @brief Check if alive
     * @return bool Whether alive
     */
    bool isAlive() const;

    /**
     * @brief Check if invisible
     * @return bool Whether invisible (crouching on grass)
     */
    bool isInvisible() const;

    // Getter methods
    Vector2D getPosition() const { return m_position; }
    Vector2D getVelocity() const { return m_velocity; }
    double getWidth() const { return GameConfig::PLAYER_WIDTH; }
    double getHeight() const { return GameConfig::PLAYER_HEIGHT; }
    int getHP() const { return m_hp; }
    int getMaxHP() const { return GameConfig::PLAYER_MAX_HP; }
    PlayerState getState() const { return m_state; }
    QColor getColor() const { return m_color; }
    bool isFacingRight() const { return m_facingRight; }
    bool getIsPlayerOne() const { return m_isPlayerOne; }
    std::shared_ptr<Weapon> getWeapon() const { return m_weapon; }

    // Setter methods
    void setPosition(const Vector2D& pos) { m_position = pos; }
    void setVelocity(const Vector2D& vel) { m_velocity = vel; }
    void setWeapon(std::shared_ptr<Weapon> weapon) { m_weapon = weapon; }
    void setGrounded(bool grounded) { m_isGrounded = grounded; }

private:
    /**
     * @brief Update physics state
     * @param deltaTime Time delta
     */
    void updatePhysics(double deltaTime);

    /**
     * @brief Update adrenaline effect
     * @param deltaTime Time delta
     */
    void updateAdrenalineEffect(double deltaTime);

    /**
     * @brief Get current movement speed
     * @return double Movement speed
     */
    double getCurrentMoveSpeed() const;

private:
    Vector2D m_position;         ///< Player position
    Vector2D m_velocity;         ///< Player velocity
    PlayerState m_state;         ///< Player state
    QColor m_color;              ///< Player color
    bool m_isPlayerOne;          ///< Whether this is player one
    bool m_facingRight;          ///< Whether facing right
    
    // Health related
    int m_hp;                    ///< Current health points
    
    // Movement control
    bool m_isMovingLeft;         ///< Whether moving left
    bool m_isMovingRight;        ///< Whether moving right
    bool m_isCrouching;          ///< Whether crouching
    bool m_isGrounded;           ///< Whether on ground
    
    // Terrain related
    TerrainType m_currentTerrain; ///< Current terrain type
    
    // Weapon system
    std::shared_ptr<Weapon> m_weapon; ///< Current weapon
    long long m_lastAttackTime;       ///< Last attack time
    
    // Status effects
    bool m_hasAdrenaline;        ///< Whether has adrenaline effect
    long long m_adrenalineEndTime; ///< Adrenaline effect end time
    long long m_lastAdrenalineHeal; ///< Last adrenaline heal time
};

#endif // PLAYER_H 