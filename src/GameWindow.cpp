/**
 * @file GameWindow.cpp
 * @brief Game main window class implementation
 * @author Justin0828
 * @date 2025-07-23
 */

#include "GameWindow.h"
#include <QPaintEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>

GameWindow::GameWindow(QWidget* parent) : QMainWindow(parent), m_frameCount(0), m_currentFPS(0.0) {
    
    // Initialize game engine
    m_gameEngine = std::make_unique<GameEngine>();
    m_gameEngine->initialize();
    
    // Initialize UI
    initializeUI();
    
    // Set window properties
    setWindowTitle("QtGame - 2D Battle Game");
    setFixedSize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    setFocusPolicy(Qt::StrongFocus);
    
    // Initialize timer
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameWindow::gameLoop);
    
    // Initialize time-related variables
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    m_fpsUpdateTime = std::chrono::high_resolution_clock::now();
    
    // Start game loop, targeting 60FPS
    int timerInterval = 1000 / GameConfig::TARGET_FPS;
    m_gameTimer->start(timerInterval);
    
    // Start game
    m_gameEngine->startGame();
}

GameWindow::~GameWindow() {
    if (m_gameTimer) {
        m_gameTimer->stop();
    }
}

void GameWindow::initializeUI() {
    // Don't set central widget, draw directly on QMainWindow
    // This way paintEvent can work properly
}

void GameWindow::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Now call complete game drawing
    drawGame(&painter);
    updateFPS();
}

void GameWindow::keyPressEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) {
        return; // Ignore auto repeat
    }
    
    Qt::Key key = static_cast<Qt::Key>(event->key());
    m_pressedKeys.insert(key);
    
    // Special key handling
    if (key == Qt::Key_R && m_gameEngine->getGameState() == GameState::GAME_OVER) {
        m_gameEngine->resetGame();
        return;
    }
    
    if (key == Qt::Key_P) {
        m_gameEngine->togglePause();
        return;
    }
    
    if (key == Qt::Key_Escape) {
        QApplication::quit();
        return;
    }
    
    m_gameEngine->handleKeyPress(key);
    QMainWindow::keyPressEvent(event);
}

void GameWindow::keyReleaseEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) {
        return; // Ignore auto repeat
    }
    
    Qt::Key key = static_cast<Qt::Key>(event->key());
    m_pressedKeys.erase(key);
    
    m_gameEngine->handleKeyRelease(key);
    QMainWindow::keyReleaseEvent(event);
}

void GameWindow::closeEvent(QCloseEvent* event) {
    if (m_gameTimer) {
        m_gameTimer->stop();
    }
    QMainWindow::closeEvent(event);
}

void GameWindow::gameLoop() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto deltaTimeMs = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastFrameTime).count();
    double deltaTime = deltaTimeMs / 1000000.0; // Convert to seconds
    m_lastFrameTime = currentTime;
    
    // Update game state
    m_gameEngine->update(deltaTime);
    
    // Repaint window
    update();
}

void GameWindow::drawGame(QPainter* painter) {
    // Draw background
    drawBackground(painter);
    
    // Draw platforms
    drawPlatforms(painter);
    
    // Draw players
    drawPlayers(painter);
    
    // Draw projectiles
    drawProjectiles(painter);
    
    // Draw items
    drawItems(painter);
    
    // Draw UI
    drawUI(painter);
    
    // Game over screen
    if (m_gameEngine->getGameState() == GameState::GAME_OVER) {
        drawGameOver(painter);
    }
}

void GameWindow::drawBackground(QPainter* painter) {
    // Gradient background
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(135, 206, 235));  // Sky blue
    gradient.setColorAt(1, QColor(176, 224, 230));  // Light blue
    
    painter->fillRect(rect(), gradient);
}

void GameWindow::drawPlatforms(QPainter* painter) {
    for (const auto& platform : m_gameEngine->getPlatforms()) {
        painter->setPen(Qt::black);
        painter->setBrush(platform.color);
        
        QRect platformRect(
            static_cast<int>(platform.position.x),
            static_cast<int>(platform.position.y),
            static_cast<int>(platform.width),
            static_cast<int>(platform.height)
        );
        
        painter->drawRect(platformRect);
        
        // Draw terrain effect hints
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 8));
        
        QString terrainText;
        switch (platform.type) {
            case TerrainType::GRASS:
                terrainText = "Grass(Stealth)";
                break;
            case TerrainType::ICE:
                terrainText = "Ice(Speed)";
                break;
            default:
                terrainText = "";
                break;
        }
        
        if (!terrainText.isEmpty() && platform.width > 80) {
            painter->drawText(platformRect, Qt::AlignCenter, terrainText);
        }
    }
}

void GameWindow::drawPlayers(QPainter* painter) {
    if (m_gameEngine->getPlayer1() && m_gameEngine->getPlayer1()->isAlive()) {
        drawPlayer(painter, m_gameEngine->getPlayer1());
    }
    
    if (m_gameEngine->getPlayer2() && m_gameEngine->getPlayer2()->isAlive()) {
        drawPlayer(painter, m_gameEngine->getPlayer2());
    }
}

void GameWindow::drawPlayer(QPainter* painter, std::shared_ptr<Player> player) {
    Vector2D pos = player->getPosition();
    QColor playerColor = player->getColor();
    
    // If player is invisible, make it semi-transparent
    if (player->isInvisible()) {
        playerColor.setAlpha(100);
    }
    
    painter->setPen(Qt::black);
    painter->setBrush(playerColor);
    
    QRect playerRect(
        static_cast<int>(pos.x),
        static_cast<int>(pos.y),
        static_cast<int>(player->getWidth()),
        static_cast<int>(player->getHeight())
    );
    
    // Adjust drawing based on state
    if (player->getState() == PlayerState::CROUCHING) {
        // Crouching state: reduce height, increase width
        playerRect.setHeight(static_cast<int>(player->getHeight() * 0.6));
        playerRect.setY(static_cast<int>(pos.y + player->getHeight() * 0.4));
        playerRect.setWidth(static_cast<int>(player->getWidth() * 1.2));
    }
    
    painter->drawRect(playerRect);
    
    // Draw facing direction indicator
    painter->setPen(QPen(Qt::white, 2));
    int eyeX = player->isFacingRight() ? 
               static_cast<int>(pos.x + player->getWidth() * 0.7) : 
               static_cast<int>(pos.x + player->getWidth() * 0.3);
    int eyeY = static_cast<int>(pos.y + player->getHeight() * 0.3);
    painter->drawEllipse(eyeX - 3, eyeY - 3, 6, 6);
    
    // Draw weapon
    if (player->getWeapon()) {
        painter->setPen(Qt::black);
        painter->setBrush(player->getWeapon()->getColor());
        
        int weaponX = player->isFacingRight() ? 
                     static_cast<int>(pos.x + player->getWidth()) : 
                     static_cast<int>(pos.x - 15);
        int weaponY = static_cast<int>(pos.y + player->getHeight() * 0.5);
        
        // Draw different shapes based on weapon type
        switch (player->getWeapon()->getType()) {
            case WeaponType::FIST:
                painter->drawEllipse(weaponX - 5, weaponY - 5, 10, 10);
                break;
            case WeaponType::KNIFE:
                painter->drawRect(weaponX - 2, weaponY - 8, 4, 16);
                break;
            case WeaponType::BALL:
                painter->drawEllipse(weaponX - 6, weaponY - 6, 12, 12);
                break;
            case WeaponType::RIFLE:
                painter->drawRect(weaponX - 15, weaponY - 3, 30, 6);
                break;
            case WeaponType::SNIPER:
                painter->drawRect(weaponX - 20, weaponY - 4, 40, 8);
                break;
        }
    }
}

void GameWindow::drawProjectiles(QPainter* painter) {
    for (const auto& projectile : m_gameEngine->getProjectiles()) {
        Vector2D pos = projectile->getPosition();
        double radius = projectile->getRadius();
        
        painter->setPen(Qt::black);
        
        // Set color based on ammunition type
        switch (projectile->getType()) {
            case AmmoType::BULLET:
                painter->setBrush(QColor(255, 255, 0)); // Yellow bullet
                break;
            case AmmoType::THROWN:
                painter->setBrush(QColor(255, 165, 0)); // Orange projectile
                break;
            default:
                painter->setBrush(Qt::red);
                break;
        }
        
        painter->drawEllipse(
            static_cast<int>(pos.x - radius),
            static_cast<int>(pos.y - radius),
            static_cast<int>(radius * 2),
            static_cast<int>(radius * 2)
        );
    }
}

void GameWindow::drawItems(QPainter* painter) {
    for (const auto& item : m_gameEngine->getItems()) {
        if (!item->isValid()) continue;
        
        Vector2D pos = item->getPosition();
        painter->setPen(Qt::black);
        painter->setBrush(item->getColor());
        
        QRect itemRect(
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            static_cast<int>(item->getWidth()),
            static_cast<int>(item->getHeight())
        );
        
        painter->drawRect(itemRect);
        
        // Draw item type text
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 8));
        
        QString itemText;
        switch (item->getType()) {
            case ItemType::WEAPON_KNIFE: itemText = "K"; break;
            case ItemType::WEAPON_BALL: itemText = "B"; break;
            case ItemType::WEAPON_RIFLE: itemText = "R"; break;
            case ItemType::WEAPON_SNIPER: itemText = "S"; break;
            case ItemType::BANDAGE: itemText = "+"; break;
            case ItemType::MEDKIT: itemText = "H"; break;
            case ItemType::ADRENALINE: itemText = "A"; break;
        }
        
        painter->drawText(itemRect, Qt::AlignCenter, itemText);
    }
}

void GameWindow::drawUI(QPainter* painter) {
    // Draw player 1 info (top left)
    drawHealthBar(painter, m_gameEngine->getPlayer1(), 20, 20, "Player 1 (ADWS+Space)");
    drawWeaponInfo(painter, m_gameEngine->getPlayer1(), 20, 80);
    
    // Draw player 2 info (top right)
    drawHealthBar(painter, m_gameEngine->getPlayer2(), width() - 220, 20, "Player 2 (JLIK+Shift)");
    drawWeaponInfo(painter, m_gameEngine->getPlayer2(), width() - 220, 80);
    
    // Draw game state
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 12));
    
    QString stateText;
    switch (m_gameEngine->getGameState()) {
        case GameState::PLAYING:
            stateText = "Game Running (P-Pause, ESC-Exit)";
            break;
        case GameState::PAUSED:
            stateText = "Game Paused (P-Resume)";
            break;
        case GameState::GAME_OVER:
            stateText = "Game Over (R-Restart)";
            break;
    }
    
    painter->drawText(width()/2 - 100, height() - 30, stateText);
}

void GameWindow::drawHealthBar(QPainter* painter, std::shared_ptr<Player> player, 
                              int x, int y, const QString& playerName) {
    if (!player) return;
    
    // Player name
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(x, y, playerName);
    
    // Health bar background
    QRect hpBarBg(x, y + 15, 200, 20);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::gray);
    painter->drawRect(hpBarBg);
    
    // Health bar
    double hpRatio = static_cast<double>(player->getHP()) / player->getMaxHP();
    int hpWidth = static_cast<int>(198 * hpRatio);
    
    QColor hpColor;
    if (hpRatio > 0.6) {
        hpColor = Qt::green;
    } else if (hpRatio > 0.3) {
        hpColor = Qt::yellow;
    } else {
        hpColor = Qt::red;
    }
    
    QRect hpBar(x + 1, y + 16, hpWidth, 18);
    painter->setBrush(hpColor);
    painter->drawRect(hpBar);
    
    // Health text
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 9, QFont::Bold));
    QString hpText = QString("%1 / %2").arg(player->getHP()).arg(player->getMaxHP());
    painter->drawText(hpBarBg, Qt::AlignCenter, hpText);
}

void GameWindow::drawWeaponInfo(QPainter* painter, std::shared_ptr<Player> player, int x, int y) {
    if (!player || !player->getWeapon()) return;
    
    auto weapon = player->getWeapon();
    
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 10));
    
    QString weaponName = getWeaponTypeName(weapon->getType());
    QString weaponInfo = QString("Weapon: %1").arg(weaponName);
    
    if (weapon->getAmmo() != -1) {
        weaponInfo += QString(" (%1 ammo)").arg(weapon->getAmmo());
    }
    
    painter->drawText(x, y, weaponInfo);
    
    // Weapon color indicator
    painter->setBrush(weapon->getColor());
    painter->drawRect(x, y + 5, 20, 10);
}

void GameWindow::drawGameOver(QPainter* painter) {
    // Semi-transparent black background
    painter->fillRect(rect(), QColor(0, 0, 0, 150));
    
    // Victory text
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 36, QFont::Bold));
    
    QString winnerText;
    if (m_gameEngine->getWinner() == 1) {
        winnerText = "Player 1 Wins!";
    } else if (m_gameEngine->getWinner() == 2) {
        winnerText = "Player 2 Wins!";
    } else {
        winnerText = "Draw!";
    }
    
    QRect textRect = rect();
    painter->drawText(textRect, Qt::AlignCenter, winnerText);
    
    // Restart hint
    painter->setFont(QFont("Arial", 16));
    painter->drawText(width()/2 - 80, height()/2 + 60, "Press R to Restart");
}

void GameWindow::drawFPS(QPainter* painter) {
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 10));
    
    QString fpsText = QString("FPS: %1").arg(static_cast<int>(m_currentFPS));
    painter->drawText(width() - 80, height() - 10, fpsText);
}

void GameWindow::updateFPS() {
    m_frameCount++;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_fpsUpdateTime).count();
    
    if (elapsed >= 1000) { // Update FPS once per second
        m_currentFPS = m_frameCount * 1000.0 / elapsed;
        m_frameCount = 0;
        m_fpsUpdateTime = currentTime;
    }
}

QString GameWindow::getWeaponTypeName(WeaponType type) {
    switch (type) {
        case WeaponType::FIST: return "Fist";
        case WeaponType::KNIFE: return "Knife";
        case WeaponType::BALL: return "Ball";
        case WeaponType::RIFLE: return "Rifle";
        case WeaponType::SNIPER: return "Sniper";
        default: return "Unknown";
    }
}

QColor GameWindow::getTerrainColor(TerrainType type) {
    switch (type) {
        case TerrainType::GROUND: return QColor(139, 69, 19);
        case TerrainType::GRASS: return QColor(34, 139, 34);
        case TerrainType::ICE: return QColor(173, 216, 230);
        default: return Qt::gray;
    }
} 