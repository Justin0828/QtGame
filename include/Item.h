/**
 * @file Item.h
 * @brief Item system class definitions
 * @author Justin0828
 * @date 2025-07-23
 */

#ifndef ITEM_H
#define ITEM_H

#include "Vector2D.h"
#include "Weapon.h"
#include <memory>
#include <QColor>

/**
 * @brief Item type enumeration
 */
enum class ItemType {
    WEAPON_KNIFE,    ///< Knife weapon
    WEAPON_BALL,     ///< Ball weapon
    WEAPON_RIFLE,    ///< Rifle weapon
    WEAPON_SNIPER,   ///< Sniper rifle weapon
    BANDAGE,         ///< Bandage
    MEDKIT,          ///< Medical kit
    ADRENALINE       ///< Adrenaline
};

/**
 * @brief Item base class
 */
class Item {
public:
    /**
     * @brief Constructor
     * @param type Item type
     * @param position Position
     */
    Item(ItemType type, const Vector2D& position);

    /**
     * @brief Virtual destructor
     */
    virtual ~Item() = default;

    /**
     * @brief Update item state
     * @param deltaTime Time delta
     */
    virtual void update(double deltaTime);

    /**
     * @brief Effect when picked up by player
     * @param player The player who picked up
     * @return bool Whether pickup was successful
     */
    virtual bool onPickup(Player* player) = 0;

    /**
     * @brief Check if item is still valid
     * @return bool Whether valid
     */
    bool isValid() const;

    // Getter methods
    ItemType getType() const { return m_type; }
    Vector2D getPosition() const { return m_position; }
    Vector2D getVelocity() const { return m_velocity; }
    double getWidth() const { return m_width; }
    double getHeight() const { return m_height; }
    QColor getColor() const { return m_color; }
    bool isGrounded() const { return m_isGrounded; }

    // Setter methods
    void setPosition(const Vector2D& pos) { m_position = pos; }
    void setVelocity(const Vector2D& vel) { m_velocity = vel; }
    void setGrounded(bool grounded) { m_isGrounded = grounded; }

protected:
    ItemType m_type;           ///< Item type
    Vector2D m_position;       ///< Position
    Vector2D m_velocity;       ///< Velocity
    double m_width;            ///< Width
    double m_height;           ///< Height
    QColor m_color;            ///< Color
    bool m_isGrounded;         ///< Whether on ground
    bool m_isValid;            ///< Whether valid
    long long m_spawnTime;     ///< Spawn time
    static constexpr long long ITEM_LIFETIME = 30000; ///< Item lifetime in milliseconds
};

/**
 * @brief Weapon item class
 */
class WeaponItem : public Item {
public:
    /**
     * @brief Constructor
     * @param type Weapon type
     * @param position Position
     */
    WeaponItem(ItemType type, const Vector2D& position);

    /**
     * @brief Create weapon when picked up
     * @param player The player who picked up
     * @return bool Whether pickup was successful
     */
    bool onPickup(Player* player) override;

private:
    /**
     * @brief Create corresponding weapon
     * @return std::shared_ptr<Weapon> Weapon object
     */
    std::shared_ptr<Weapon> createWeapon();
};

/**
 * @brief Bandage item class
 */
class BandageItem : public Item {
public:
    /**
     * @brief Constructor
     * @param position Position
     */
    explicit BandageItem(const Vector2D& position);

    /**
     * @brief Restore small amount of health when picked up
     * @param player The player who picked up
     * @return bool Whether pickup was successful
     */
    bool onPickup(Player* player) override;
};

/**
 * @brief Medical kit item class
 */
class MedkitItem : public Item {
public:
    /**
     * @brief Constructor
     * @param position Position
     */
    explicit MedkitItem(const Vector2D& position);

    /**
     * @brief Restore full health when picked up
     * @param player The player who picked up
     * @return bool Whether pickup was successful
     */
    bool onPickup(Player* player) override;
};

/**
 * @brief Adrenaline item class
 */
class AdrenalineItem : public Item {
public:
    /**
     * @brief Constructor
     * @param position Position
     */
    explicit AdrenalineItem(const Vector2D& position);

    /**
     * @brief Apply adrenaline effect when picked up
     * @param player The player who picked up
     * @return bool Whether pickup was successful
     */
    bool onPickup(Player* player) override;
};

#endif // ITEM_H 