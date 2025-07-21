/**
 * @file Item.h
 * @brief 物品系统类定义
 * @author QtGame Team
 * @date 2024
 */

#ifndef ITEM_H
#define ITEM_H

#include "Vector2D.h"
#include "Weapon.h"
#include <memory>
#include <QColor>

/**
 * @brief 物品类型枚举
 */
enum class ItemType {
    WEAPON_KNIFE,    ///< 小刀武器
    WEAPON_BALL,     ///< 实心球武器
    WEAPON_RIFLE,    ///< 步枪武器
    WEAPON_SNIPER,   ///< 狙击枪武器
    BANDAGE,         ///< 绷带
    MEDKIT,          ///< 医疗箱
    ADRENALINE       ///< 肾上腺素
};

/**
 * @brief 物品基类
 */
class Item {
public:
    /**
     * @brief 构造函数
     * @param type 物品类型
     * @param position 位置
     */
    Item(ItemType type, const Vector2D& position);

    /**
     * @brief 虚析构函数
     */
    virtual ~Item() = default;

    /**
     * @brief 更新物品状态
     * @param deltaTime 时间增量
     */
    virtual void update(double deltaTime);

    /**
     * @brief 被玩家拾取时的效果
     * @param player 拾取的玩家
     * @return bool 是否成功拾取
     */
    virtual bool onPickup(Player* player) = 0;

    /**
     * @brief 检查物品是否仍然有效
     * @return bool 是否有效
     */
    bool isValid() const;

    // Getter 方法
    ItemType getType() const { return m_type; }
    Vector2D getPosition() const { return m_position; }
    Vector2D getVelocity() const { return m_velocity; }
    double getWidth() const { return m_width; }
    double getHeight() const { return m_height; }
    QColor getColor() const { return m_color; }
    bool isGrounded() const { return m_isGrounded; }

    // Setter 方法
    void setPosition(const Vector2D& pos) { m_position = pos; }
    void setVelocity(const Vector2D& vel) { m_velocity = vel; }
    void setGrounded(bool grounded) { m_isGrounded = grounded; }

protected:
    ItemType m_type;           ///< 物品类型
    Vector2D m_position;       ///< 位置
    Vector2D m_velocity;       ///< 速度
    double m_width;            ///< 宽度
    double m_height;           ///< 高度
    QColor m_color;            ///< 颜色
    bool m_isGrounded;         ///< 是否在地面
    bool m_isValid;            ///< 是否有效
    long long m_spawnTime;     ///< 生成时间
    static constexpr long long ITEM_LIFETIME = 30000; ///< 物品存在时间（毫秒）
};

/**
 * @brief 武器物品类
 */
class WeaponItem : public Item {
public:
    /**
     * @brief 构造函数
     * @param type 武器类型
     * @param position 位置
     */
    WeaponItem(ItemType type, const Vector2D& position);

    /**
     * @brief 被拾取时创建武器
     * @param player 拾取的玩家
     * @return bool 是否成功拾取
     */
    bool onPickup(Player* player) override;

private:
    /**
     * @brief 创建对应的武器
     * @return std::shared_ptr<Weapon> 武器对象
     */
    std::shared_ptr<Weapon> createWeapon();
};

/**
 * @brief 绷带物品类
 */
class BandageItem : public Item {
public:
    /**
     * @brief 构造函数
     * @param position 位置
     */
    explicit BandageItem(const Vector2D& position);

    /**
     * @brief 被拾取时恢复少量血量
     * @param player 拾取的玩家
     * @return bool 是否成功拾取
     */
    bool onPickup(Player* player) override;
};

/**
 * @brief 医疗箱物品类
 */
class MedkitItem : public Item {
public:
    /**
     * @brief 构造函数
     * @param position 位置
     */
    explicit MedkitItem(const Vector2D& position);

    /**
     * @brief 被拾取时恢复全部血量
     * @param player 拾取的玩家
     * @return bool 是否成功拾取
     */
    bool onPickup(Player* player) override;
};

/**
 * @brief 肾上腺素物品类
 */
class AdrenalineItem : public Item {
public:
    /**
     * @brief 构造函数
     * @param position 位置
     */
    explicit AdrenalineItem(const Vector2D& position);

    /**
     * @brief 被拾取时给予肾上腺素效果
     * @param player 拾取的玩家
     * @return bool 是否成功拾取
     */
    bool onPickup(Player* player) override;
};

#endif // ITEM_H 