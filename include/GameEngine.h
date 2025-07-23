/**
 * @file GameEngine.h
 * @brief Game engine class definition
 * @author Justin0828
 * @date 2025-07-23
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
 * @brief Game state enumeration
 */
enum class GameState {
    PLAYING,    ///< Game in progress
    PAUSED,     ///< Game paused
    GAME_OVER   ///< Game over
};

/**
 * @brief Platform structure
 */
struct Platform {
    Vector2D position;    ///< Position
    double width;         ///< Width
    double height;        ///< Height
    TerrainType type;     ///< Terrain type
    QColor color;         ///< Color
    
    /**
     * @brief Constructor
     */
    Platform(const Vector2D& pos, double w, double h, TerrainType t, const QColor& c)
        : position(pos), width(w), height(h), type(t), color(c) {}
};

/**
 * @brief Game engine class
 * 
 * Responsible for core game logic including player management, physics system, collision detection, etc.
 */
class GameEngine : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    explicit GameEngine(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~GameEngine();

    /**
     * @brief Initialize the game
     */
    void initialize();

    /**
     * @brief Start the game
     */
    void startGame();

    /**
     * @brief Toggle pause/resume game
     */
    void togglePause();

    /**
     * @brief Reset the game
     */
    void resetGame();

    /**
     * @brief Update game state
     * @param deltaTime Time delta
     */
    void update(double deltaTime);

    /**
     * @brief Handle key press events
     * @param key The pressed key
     */
    void handleKeyPress(Qt::Key key);

    /**
     * @brief Handle key release events
     * @param key The released key
     */
    void handleKeyRelease(Qt::Key key);

    // Getter methods
    GameState getGameState() const { return m_gameState; }
    std::shared_ptr<Player> getPlayer1() const { return m_player1; }
    std::shared_ptr<Player> getPlayer2() const { return m_player2; }
    const std::vector<std::shared_ptr<Projectile>>& getProjectiles() const { return m_projectiles; }
    const std::vector<std::shared_ptr<Item>>& getItems() const { return m_items; }
    const std::vector<Platform>& getPlatforms() const { return m_platforms; }
    int getWinner() const { return m_winner; } // 0: no winner, 1: player1, 2: player2

private slots:
    /**
     * @brief Spawn random items
     */
    void spawnRandomItem();

private:
    /**
     * @brief Create map platforms
     */
    void createPlatforms();

    /**
     * @brief Update physics system
     * @param deltaTime Time delta
     */
    void updatePhysics(double deltaTime);

    /**
     * @brief Update projectiles
     * @param deltaTime Time delta
     */
    void updateProjectiles(double deltaTime);

    /**
     * @brief Update items
     * @param deltaTime Time delta
     */
    void updateItems(double deltaTime);

    /**
     * @brief Check collisions
     */
    void checkCollisions();

    /**
     * @brief Check player-platform collision
     * @param player The player
     */
    void checkPlayerPlatformCollision(std::shared_ptr<Player> player);

    /**
     * @brief Check projectile-player collision
     */
    void checkProjectilePlayerCollision();

    /**
     * @brief Check projectile-platform collision
     */
    void checkProjectilePlatformCollision();

    /**
     * @brief Check player-item collision
     */
    void checkPlayerItemCollision();

    /**
     * @brief Handle player attack
     * @param player The attacking player
     */
    void handlePlayerAttack(std::shared_ptr<Player> player);

    /**
     * @brief Check collision between two rectangles
     * @param pos1 First rectangle position
     * @param size1 First rectangle size
     * @param pos2 Second rectangle position
     * @param size2 Second rectangle size
     * @return bool Whether collision occurs
     */
    bool checkRectCollision(const Vector2D& pos1, const Vector2D& size1,
                           const Vector2D& pos2, const Vector2D& size2);

    /**
     * @brief Check collision between circle and rectangle
     * @param circlePos Circle center position
     * @param radius Circle radius
     * @param rectPos Rectangle position
     * @param rectSize Rectangle size
     * @return bool Whether collision occurs
     */
    bool checkCircleRectCollision(const Vector2D& circlePos, double radius,
                                 const Vector2D& rectPos, const Vector2D& rectSize);

    /**
     * @brief Get the terrain type where player is standing
     * @param player The player
     * @return TerrainType Terrain type
     */
    TerrainType getPlayerTerrainType(std::shared_ptr<Player> player);

    /**
     * @brief Generate random item type
     * @return ItemType Item type
     */
    ItemType generateRandomItemType();

    /**
     * @brief Generate random drop position
     * @return Vector2D Drop position
     */
    Vector2D generateRandomDropPosition();

private:
    GameState m_gameState;                                    ///< Game state
    std::shared_ptr<Player> m_player1;                       ///< Player 1
    std::shared_ptr<Player> m_player2;                       ///< Player 2
    std::vector<std::shared_ptr<Projectile>> m_projectiles; ///< Projectile list
    std::vector<std::shared_ptr<Item>> m_items;             ///< Item list
    std::vector<Platform> m_platforms;                       ///< Platform list
    int m_winner;                                             ///< Winner

    // Random number generator
    std::random_device m_randomDevice;                       ///< Random device
    std::mt19937 m_randomGenerator;                          ///< Random number generator

    // Item drop timer
    QTimer* m_itemDropTimer;                                 ///< Item drop timer
};

#endif // GAMEENGINE_H 