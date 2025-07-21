/**
 * @file GameEngine.h
 * @brief 游戏引擎类定义
 * @author QtGame Team
 * @date 2024
 */

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Player.h"
#include "Weapon.h"
#include "Item.h"
#include "Vector2D.h"
#include <vector>
#include <memory>
#include <random>
#include <QTimer>
#include <QObject>

/**
 * @brief 游戏状态枚举
 */
enum class GameState {
    PLAYING,    ///< 游戏进行中
    PAUSED,     ///< 游戏暂停
    GAME_OVER   ///< 游戏结束
};

/**
 * @brief 平台结构体
 */
struct Platform {
    Vector2D position;    ///< 位置
    double width;         ///< 宽度
    double height;        ///< 高度
    TerrainType type;     ///< 地形类型
    QColor color;         ///< 颜色
    
    /**
     * @brief 构造函数
     */
    Platform(const Vector2D& pos, double w, double h, TerrainType t, const QColor& c)
        : position(pos), width(w), height(h), type(t), color(c) {}
};

/**
 * @brief 游戏引擎类
 * 
 * 负责游戏的核心逻辑，包括玩家管理、物理系统、碰撞检测等
 */
class GameEngine : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     */
    explicit GameEngine(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~GameEngine();

    /**
     * @brief 初始化游戏
     */
    void initialize();

    /**
     * @brief 开始游戏
     */
    void startGame();

    /**
     * @brief 暂停/继续游戏
     */
    void togglePause();

    /**
     * @brief 重置游戏
     */
    void resetGame();

    /**
     * @brief 更新游戏状态
     * @param deltaTime 时间增量
     */
    void update(double deltaTime);

    /**
     * @brief 处理按键按下事件
     * @param key 按键
     */
    void handleKeyPress(Qt::Key key);

    /**
     * @brief 处理按键释放事件
     * @param key 按键
     */
    void handleKeyRelease(Qt::Key key);

    // Getter 方法
    GameState getGameState() const { return m_gameState; }
    std::shared_ptr<Player> getPlayer1() const { return m_player1; }
    std::shared_ptr<Player> getPlayer2() const { return m_player2; }
    const std::vector<std::shared_ptr<Projectile>>& getProjectiles() const { return m_projectiles; }
    const std::vector<std::shared_ptr<Item>>& getItems() const { return m_items; }
    const std::vector<Platform>& getPlatforms() const { return m_platforms; }
    int getWinner() const { return m_winner; } // 0: 无胜者, 1: 玩家1, 2: 玩家2

private slots:
    /**
     * @brief 生成随机物品
     */
    void spawnRandomItem();

private:
    /**
     * @brief 创建地图平台
     */
    void createPlatforms();

    /**
     * @brief 更新物理系统
     * @param deltaTime 时间增量
     */
    void updatePhysics(double deltaTime);

    /**
     * @brief 更新投射物
     * @param deltaTime 时间增量
     */
    void updateProjectiles(double deltaTime);

    /**
     * @brief 更新物品
     * @param deltaTime 时间增量
     */
    void updateItems(double deltaTime);

    /**
     * @brief 检测碰撞
     */
    void checkCollisions();

    /**
     * @brief 检测玩家与平台碰撞
     * @param player 玩家
     */
    void checkPlayerPlatformCollision(std::shared_ptr<Player> player);

    /**
     * @brief 检测投射物与玩家碰撞
     */
    void checkProjectilePlayerCollision();

    /**
     * @brief 检测投射物与平台碰撞
     */
    void checkProjectilePlatformCollision();

    /**
     * @brief 检测玩家与物品碰撞
     */
    void checkPlayerItemCollision();

    /**
     * @brief 处理玩家攻击
     * @param player 攻击的玩家
     */
    void handlePlayerAttack(std::shared_ptr<Player> player);

    /**
     * @brief 检测两个矩形是否碰撞
     * @param pos1 第一个矩形位置
     * @param size1 第一个矩形大小
     * @param pos2 第二个矩形位置
     * @param size2 第二个矩形大小
     * @return bool 是否碰撞
     */
    bool checkRectCollision(const Vector2D& pos1, const Vector2D& size1,
                           const Vector2D& pos2, const Vector2D& size2);

    /**
     * @brief 检测圆形与矩形碰撞
     * @param circlePos 圆心位置
     * @param radius 半径
     * @param rectPos 矩形位置
     * @param rectSize 矩形大小
     * @return bool 是否碰撞
     */
    bool checkCircleRectCollision(const Vector2D& circlePos, double radius,
                                 const Vector2D& rectPos, const Vector2D& rectSize);

    /**
     * @brief 获取玩家当前所在地形类型
     * @param player 玩家
     * @return TerrainType 地形类型
     */
    TerrainType getPlayerTerrainType(std::shared_ptr<Player> player);

    /**
     * @brief 生成随机物品类型
     * @return ItemType 物品类型
     */
    ItemType generateRandomItemType();

    /**
     * @brief 生成随机掉落位置
     * @return Vector2D 掉落位置
     */
    Vector2D generateRandomDropPosition();

private:
    GameState m_gameState;                                    ///< 游戏状态
    std::shared_ptr<Player> m_player1;                       ///< 玩家1
    std::shared_ptr<Player> m_player2;                       ///< 玩家2
    std::vector<std::shared_ptr<Projectile>> m_projectiles; ///< 投射物列表
    std::vector<std::shared_ptr<Item>> m_items;             ///< 物品列表
    std::vector<Platform> m_platforms;                       ///< 平台列表
    int m_winner;                                             ///< 胜利者

    // 随机数生成器
    std::random_device m_randomDevice;                       ///< 随机设备
    std::mt19937 m_randomGenerator;                          ///< 随机数生成器

    // 物品掉落定时器
    QTimer* m_itemDropTimer;                                 ///< 物品掉落定时器
};

#endif // GAMEENGINE_H 