/**
 * @file GameWindow.cpp
 * @brief 游戏主窗口类实现
 * @author QtGame Team
 * @date 2024
 */

#include "GameWindow.h"
#include <QPaintEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>

GameWindow::GameWindow(QWidget* parent) : QMainWindow(parent), m_frameCount(0), m_currentFPS(0.0) {
    
    // 初始化游戏引擎
    m_gameEngine = std::make_unique<GameEngine>();
    m_gameEngine->initialize();
    
    // 初始化UI
    initializeUI();
    
    // 设置窗口属性
    setWindowTitle("QtGame - 2D对战游戏");
    setFixedSize(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT);
    setFocusPolicy(Qt::StrongFocus);
    
    // 初始化计时器
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameWindow::gameLoop);
    
    // 初始化时间相关变量
    m_lastFrameTime = std::chrono::high_resolution_clock::now();
    m_fpsUpdateTime = std::chrono::high_resolution_clock::now();
    
    // 启动游戏循环，目标60FPS
    int timerInterval = 1000 / GameConfig::TARGET_FPS;
    m_gameTimer->start(timerInterval);
    
    // 启动游戏
    m_gameEngine->startGame();
}

GameWindow::~GameWindow() {
    if (m_gameTimer) {
        m_gameTimer->stop();
    }
}

void GameWindow::initializeUI() {
    // 不设置中央窗口部件，直接在QMainWindow上绘制
    // 这样paintEvent才能正常工作
}

void GameWindow::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 现在调用完整的游戏绘制
    drawGame(&painter);
    updateFPS();
}

void GameWindow::keyPressEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) {
        return; // 忽略自动重复
    }
    
    Qt::Key key = static_cast<Qt::Key>(event->key());
    m_pressedKeys.insert(key);
    
    // 特殊按键处理
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
        return; // 忽略自动重复
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
    double deltaTime = deltaTimeMs / 1000000.0; // 转换为秒
    m_lastFrameTime = currentTime;
    
    // 更新游戏状态
    m_gameEngine->update(deltaTime);
    
    // 重绘窗口
    update();
}

void GameWindow::drawGame(QPainter* painter) {
    // 绘制背景
    drawBackground(painter);
    
    // 绘制平台
    drawPlatforms(painter);
    
    // 绘制玩家
    drawPlayers(painter);
    
    // 绘制投射物
    drawProjectiles(painter);
    
    // 绘制物品
    drawItems(painter);
    
    // 绘制UI
    drawUI(painter);
    
    // 游戏结束画面
    if (m_gameEngine->getGameState() == GameState::GAME_OVER) {
        drawGameOver(painter);
    }
}

void GameWindow::drawBackground(QPainter* painter) {
    // 渐变背景
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(135, 206, 235));  // 天蓝色
    gradient.setColorAt(1, QColor(176, 224, 230));  // 浅蓝色
    
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
        
        // 绘制地形效果提示
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 8));
        
        QString terrainText;
        switch (platform.type) {
            case TerrainType::GRASS:
                terrainText = "草地(隐身)";
                break;
            case TerrainType::ICE:
                terrainText = "冰面(加速)";
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
    
    // 如果玩家隐身，使其半透明
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
    
    // 根据状态调整绘制
    if (player->getState() == PlayerState::CROUCHING) {
        // 下蹲状态：减小高度，增加宽度
        playerRect.setHeight(static_cast<int>(player->getHeight() * 0.6));
        playerRect.setY(static_cast<int>(pos.y + player->getHeight() * 0.4));
        playerRect.setWidth(static_cast<int>(player->getWidth() * 1.2));
    }
    
    painter->drawRect(playerRect);
    
    // 绘制面向方向指示
    painter->setPen(QPen(Qt::white, 2));
    int eyeX = player->isFacingRight() ? 
               static_cast<int>(pos.x + player->getWidth() * 0.7) : 
               static_cast<int>(pos.x + player->getWidth() * 0.3);
    int eyeY = static_cast<int>(pos.y + player->getHeight() * 0.3);
    painter->drawEllipse(eyeX - 3, eyeY - 3, 6, 6);
    
    // 绘制武器
    if (player->getWeapon()) {
        painter->setPen(Qt::black);
        painter->setBrush(player->getWeapon()->getColor());
        
        int weaponX = player->isFacingRight() ? 
                     static_cast<int>(pos.x + player->getWidth()) : 
                     static_cast<int>(pos.x - 15);
        int weaponY = static_cast<int>(pos.y + player->getHeight() * 0.5);
        
        // 根据武器类型绘制不同形状
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
        
        // 根据弹药类型设置颜色
        switch (projectile->getType()) {
            case AmmoType::BULLET:
                painter->setBrush(QColor(255, 255, 0)); // 黄色子弹
                break;
            case AmmoType::THROWN:
                painter->setBrush(QColor(255, 165, 0)); // 橙色投掷物
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
        
        // 绘制物品类型文字
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 8));
        
        QString itemText;
        switch (item->getType()) {
            case ItemType::WEAPON_KNIFE: itemText = "刀"; break;
            case ItemType::WEAPON_BALL: itemText = "球"; break;
            case ItemType::WEAPON_RIFLE: itemText = "步"; break;
            case ItemType::WEAPON_SNIPER: itemText = "狙"; break;
            case ItemType::BANDAGE: itemText = "+"; break;
            case ItemType::MEDKIT: itemText = "医"; break;
            case ItemType::ADRENALINE: itemText = "肾"; break;
        }
        
        painter->drawText(itemRect, Qt::AlignCenter, itemText);
    }
}

void GameWindow::drawUI(QPainter* painter) {
    // 绘制玩家1信息（左上角）
    drawHealthBar(painter, m_gameEngine->getPlayer1(), 20, 20, "玩家1 (ADWS+空格)");
    drawWeaponInfo(painter, m_gameEngine->getPlayer1(), 20, 80);
    
    // 绘制玩家2信息（右上角）
    drawHealthBar(painter, m_gameEngine->getPlayer2(), width() - 220, 20, "玩家2 (JLIK+Shift)");
    drawWeaponInfo(painter, m_gameEngine->getPlayer2(), width() - 220, 80);
    
    // 绘制游戏状态
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 12));
    
    QString stateText;
    switch (m_gameEngine->getGameState()) {
        case GameState::PLAYING:
            stateText = "游戏进行中 (P-暂停, ESC-退出)";
            break;
        case GameState::PAUSED:
            stateText = "游戏已暂停 (P-继续)";
            break;
        case GameState::GAME_OVER:
            stateText = "游戏结束 (R-重新开始)";
            break;
    }
    
    painter->drawText(width()/2 - 100, height() - 30, stateText);
}

void GameWindow::drawHealthBar(QPainter* painter, std::shared_ptr<Player> player, 
                              int x, int y, const QString& playerName) {
    if (!player) return;
    
    // 玩家名称
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(x, y, playerName);
    
    // 生命值条背景
    QRect hpBarBg(x, y + 15, 200, 20);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::gray);
    painter->drawRect(hpBarBg);
    
    // 生命值条
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
    
    // 生命值文字
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
    QString weaponInfo = QString("武器: %1").arg(weaponName);
    
    if (weapon->getAmmo() != -1) {
        weaponInfo += QString(" (%1发)").arg(weapon->getAmmo());
    }
    
    painter->drawText(x, y, weaponInfo);
    
    // 武器颜色指示
    painter->setBrush(weapon->getColor());
    painter->drawRect(x, y + 5, 20, 10);
}

void GameWindow::drawGameOver(QPainter* painter) {
    // 半透明黑色背景
    painter->fillRect(rect(), QColor(0, 0, 0, 150));
    
    // 胜利文字
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 36, QFont::Bold));
    
    QString winnerText;
    if (m_gameEngine->getWinner() == 1) {
        winnerText = "玩家1 胜利!";
    } else if (m_gameEngine->getWinner() == 2) {
        winnerText = "玩家2 胜利!";
    } else {
        winnerText = "平局!";
    }
    
    QRect textRect = rect();
    painter->drawText(textRect, Qt::AlignCenter, winnerText);
    
    // 重新开始提示
    painter->setFont(QFont("Arial", 16));
    painter->drawText(width()/2 - 80, height()/2 + 60, "按 R 键重新开始");
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
    
    if (elapsed >= 1000) { // 每秒更新一次FPS
        m_currentFPS = m_frameCount * 1000.0 / elapsed;
        m_frameCount = 0;
        m_fpsUpdateTime = currentTime;
    }
}

QString GameWindow::getWeaponTypeName(WeaponType type) {
    switch (type) {
        case WeaponType::FIST: return "拳头";
        case WeaponType::KNIFE: return "小刀";
        case WeaponType::BALL: return "实心球";
        case WeaponType::RIFLE: return "步枪";
        case WeaponType::SNIPER: return "狙击枪";
        default: return "未知";
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