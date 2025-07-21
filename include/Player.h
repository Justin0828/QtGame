/**
 * @file Player.h
 * @brief 玩家类定义
 * @author QtGame Team
 * @date 2024
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "Vector2D.h"
#include "GameConfig.h"
#include <memory>
#include <QColor>

// 前向声明
class Weapon;
class Item;

/**
 * @brief 玩家状态枚举
 */
enum class PlayerState {
    STANDING,   ///< 站立状态
    MOVING,     ///< 移动状态
    JUMPING,    ///< 跳跃状态
    CROUCHING,  ///< 下蹲状态
    ATTACKING   ///< 攻击状态
};

/**
 * @brief 地形类型枚举
 */
enum class TerrainType {
    GROUND,     ///< 普通土地
    GRASS,      ///< 草地（隐身效果）
    ICE         ///< 冰场（加速效果）
};

/**
 * @brief 玩家类
 * 
 * 处理玩家的移动、攻击、生命值等所有功能
 */
class Player {
public:
    /**
     * @brief 构造函数
     * @param startPos 初始位置
     * @param playerColor 玩家颜色
     * @param isPlayerOne 是否为玩家1
     */
    Player(const Vector2D& startPos, const QColor& playerColor, bool isPlayerOne);

    /**
     * @brief 析构函数
     */
    ~Player();

    /**
     * @brief 更新玩家状态
     * @param deltaTime 时间增量（秒）
     */
    void update(double deltaTime);

    /**
     * @brief 向左移动
     */
    void moveLeft();

    /**
     * @brief 向右移动
     */
    void moveRight();

    /**
     * @brief 停止移动
     */
    void stopMoving();

    /**
     * @brief 跳跃
     */
    void jump();

    /**
     * @brief 下蹲
     */
    void crouch();

    /**
     * @brief 停止下蹲
     */
    void stopCrouching();

    /**
     * @brief 攻击
     */
    void attack();

    /**
     * @brief 拾取物品
     * @param item 要拾取的物品
     * @return bool 是否成功拾取
     */
    bool pickupItem(std::shared_ptr<Item> item);

    /**
     * @brief 受到伤害
     * @param damage 伤害值
     */
    void takeDamage(int damage);

    /**
     * @brief 治疗
     * @param healAmount 治疗量
     */
    void heal(int healAmount);

    /**
     * @brief 应用肾上腺素效果
     * @param duration 持续时间（毫秒）
     */
    void applyAdrenaline(int duration);

    /**
     * @brief 设置地形类型
     * @param terrain 地形类型
     */
    void setTerrainType(TerrainType terrain);

    /**
     * @brief 检查是否在地面上
     * @return bool 是否在地面
     */
    bool isGrounded() const;

    /**
     * @brief 检查是否存活
     * @return bool 是否存活
     */
    bool isAlive() const;

    /**
     * @brief 检查是否隐身
     * @return bool 是否隐身（草地下蹲状态）
     */
    bool isInvisible() const;

    // Getter 方法
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

    // Setter 方法
    void setPosition(const Vector2D& pos) { m_position = pos; }
    void setVelocity(const Vector2D& vel) { m_velocity = vel; }
    void setWeapon(std::shared_ptr<Weapon> weapon) { m_weapon = weapon; }

private:
    /**
     * @brief 更新物理状态
     * @param deltaTime 时间增量
     */
    void updatePhysics(double deltaTime);

    /**
     * @brief 更新肾上腺素效果
     * @param deltaTime 时间增量
     */
    void updateAdrenalineEffect(double deltaTime);

    /**
     * @brief 获取当前移动速度
     * @return double 移动速度
     */
    double getCurrentMoveSpeed() const;

private:
    Vector2D m_position;         ///< 玩家位置
    Vector2D m_velocity;         ///< 玩家速度
    PlayerState m_state;         ///< 玩家状态
    QColor m_color;              ///< 玩家颜色
    bool m_isPlayerOne;          ///< 是否为玩家1
    bool m_facingRight;          ///< 是否面向右方
    
    // 生命值相关
    int m_hp;                    ///< 当前生命值
    
    // 移动控制
    bool m_isMovingLeft;         ///< 是否正在向左移动
    bool m_isMovingRight;        ///< 是否正在向右移动
    bool m_isCrouching;          ///< 是否正在下蹲
    bool m_isGrounded;           ///< 是否在地面上
    
    // 地形相关
    TerrainType m_currentTerrain; ///< 当前所在地形
    
    // 武器系统
    std::shared_ptr<Weapon> m_weapon; ///< 当前武器
    long long m_lastAttackTime;       ///< 上次攻击时间
    
    // 状态效果
    bool m_hasAdrenaline;        ///< 是否有肾上腺素效果
    long long m_adrenalineEndTime; ///< 肾上腺素效果结束时间
    long long m_lastAdrenalineHeal; ///< 上次肾上腺素回血时间
};

#endif // PLAYER_H 