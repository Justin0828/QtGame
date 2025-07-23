/**
 * @file GameWindow.h
 * @brief Game main window class definition
 * @author Justin0828
 * @date 2025-07-23
 */

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "GameEngine.h"
#include "GameConfig.h"
#include <QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <chrono>
#include <set>

/**
 * @brief Game main window class
 * 
 * Inherits from QMainWindow, responsible for game rendering and user interface
 */
class GameWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent window
     */
    explicit GameWindow(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~GameWindow();

protected:
    /**
     * @brief Paint event
     * @param event Paint event
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief Key press event
     * @param event Key event
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief Key release event
     * @param event Key event
     */
    void keyReleaseEvent(QKeyEvent* event) override;

    /**
     * @brief Window close event
     * @param event Close event
     */
    void closeEvent(QCloseEvent* event) override;

private slots:
    /**
     * @brief Game loop update
     */
    void gameLoop();

private:
    /**
     * @brief Initialize UI interface
     */
    void initializeUI();

    /**
     * @brief Draw game elements
     * @param painter Painter object
     */
    void drawGame(QPainter* painter);

    /**
     * @brief Draw background
     * @param painter Painter object
     */
    void drawBackground(QPainter* painter);

    /**
     * @brief Draw platforms
     * @param painter Painter object
     */
    void drawPlatforms(QPainter* painter);

    /**
     * @brief Draw players
     * @param painter Painter object
     */
    void drawPlayers(QPainter* painter);

    /**
     * @brief Draw single player
     * @param painter Painter object
     * @param player Player object
     */
    void drawPlayer(QPainter* painter, std::shared_ptr<Player> player);

    /**
     * @brief Draw projectiles
     * @param painter Painter object
     */
    void drawProjectiles(QPainter* painter);

    /**
     * @brief Draw items
     * @param painter Painter object
     */
    void drawItems(QPainter* painter);

    /**
     * @brief Draw UI interface
     * @param painter Painter object
     */
    void drawUI(QPainter* painter);

    /**
     * @brief Draw health bar
     * @param painter Painter object
     * @param player Player object
     * @param x X coordinate
     * @param y Y coordinate
     * @param playerName Player name
     */
    void drawHealthBar(QPainter* painter, std::shared_ptr<Player> player, 
                      int x, int y, const QString& playerName);

    /**
     * @brief Draw weapon information
     * @param painter Painter object
     * @param player Player object
     * @param x X coordinate
     * @param y Y coordinate
     */
    void drawWeaponInfo(QPainter* painter, std::shared_ptr<Player> player, int x, int y);

    /**
     * @brief Draw game over screen
     * @param painter Painter object
     */
    void drawGameOver(QPainter* painter);

    /**
     * @brief Draw FPS information
     * @param painter Painter object
     */
    void drawFPS(QPainter* painter);

    /**
     * @brief Update FPS calculation
     */
    void updateFPS();

    /**
     * @brief Get weapon type name
     * @param type Weapon type
     * @return QString Weapon name
     */
    QString getWeaponTypeName(WeaponType type);

    /**
     * @brief Get terrain color
     * @param type Terrain type
     * @return QColor Terrain color
     */
    QColor getTerrainColor(TerrainType type);

private:
    std::unique_ptr<GameEngine> m_gameEngine; ///< Game engine
    QTimer* m_gameTimer;                      ///< Game loop timer
    
    // FPS calculation
    std::chrono::high_resolution_clock::time_point m_lastFrameTime; ///< Last frame time
    int m_frameCount;                         ///< Frame count
    double m_currentFPS;                      ///< Current FPS
    std::chrono::high_resolution_clock::time_point m_fpsUpdateTime; ///< FPS update time

    // UI elements
    QWidget* m_centralWidget;                 ///< Central widget
    
    // Key state tracking
    std::set<Qt::Key> m_pressedKeys;          ///< Currently pressed keys set
};

#endif // GAMEWINDOW_H 