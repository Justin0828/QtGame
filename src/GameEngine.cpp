/**
 * @file GameEngine.cpp
 * @brief 游戏引擎类实现
 * @author QtGame Team
 * @date 2024
 */

#include "GameEngine.h"
#include "Item.h"
#include <algorithm>
#include <cmath>
#include <QDebug>

GameEngine::GameEngine(QObject* parent) 
    : QObject(parent), m_gameState(GameState::PLAYING), m_winner(0), m_randomGenerator(m_randomDevice()) {
    
    m_itemDropTimer = new QTimer(this);
    connect(m_itemDropTimer, &QTimer::timeout, this, &GameEngine::spawnRandomItem);
}

GameEngine::~GameEngine() {
    if (m_itemDropTimer) {
        m_itemDropTimer->stop();
    }
}

void GameEngine::initialize() {
    // 创建玩家
    Vector2D player1StartPos(200, GameConfig::GROUND_LEVEL - GameConfig::PLAYER_HEIGHT);
    Vector2D player2StartPos(1000, GameConfig::GROUND_LEVEL - GameConfig::PLAYER_HEIGHT);
    
    m_player1 = std::make_shared<Player>(player1StartPos, QColor(0, 0, 255), true);  // 蓝色玩家1
    m_player2 = std::make_shared<Player>(player2StartPos, QColor(255, 0, 0), false); // 红色玩家2
    
    // 创建地图
    createPlatforms();
    
    // 重置游戏状态
    m_gameState = GameState::PLAYING;
    m_winner = 0;
    
    // 清空列表
    m_projectiles.clear();
    m_items.clear();
}

void GameEngine::startGame() {
    if (m_gameState != GameState::PLAYING) {
        m_gameState = GameState::PLAYING;
    }
    
    // 启动物品掉落计时器
    m_itemDropTimer->start(GameConfig::ITEM_DROP_INTERVAL);
}

void GameEngine::togglePause() {
    if (m_gameState == GameState::PLAYING) {
        m_gameState = GameState::PAUSED;
        m_itemDropTimer->stop();
    } else if (m_gameState == GameState::PAUSED) {
        m_gameState = GameState::PLAYING;
        m_itemDropTimer->start(GameConfig::ITEM_DROP_INTERVAL);
    }
}

void GameEngine::resetGame() {
    initialize();
    startGame();
}

void GameEngine::update(double deltaTime) {
    if (m_gameState != GameState::PLAYING) return;
    
    // 更新玩家
    if (m_player1 && m_player1->isAlive()) {
        m_player1->update(deltaTime);
        m_player1->setTerrainType(getPlayerTerrainType(m_player1));
    }
    
    if (m_player2 && m_player2->isAlive()) {
        m_player2->update(deltaTime);
        m_player2->setTerrainType(getPlayerTerrainType(m_player2));
    }
    
    // 更新物理系统
    updatePhysics(deltaTime);
    
    // 更新投射物
    updateProjectiles(deltaTime);
    
    // 更新物品
    updateItems(deltaTime);
    
    // 检测碰撞
    checkCollisions();
    
    // 检查游戏结束条件
    if (!m_player1->isAlive()) {
        m_winner = 2;
        m_gameState = GameState::GAME_OVER;
        m_itemDropTimer->stop();
    } else if (!m_player2->isAlive()) {
        m_winner = 1;
        m_gameState = GameState::GAME_OVER;
        m_itemDropTimer->stop();
    }
}

void GameEngine::handleKeyPress(Qt::Key key) {
    if (m_gameState != GameState::PLAYING) return;
    
    // 玩家1控制
    if (key == GameConfig::PLAYER1_LEFT) {
        m_player1->moveLeft();
    } else if (key == GameConfig::PLAYER1_RIGHT) {
        m_player1->moveRight();
    } else if (key == GameConfig::PLAYER1_JUMP) {
        m_player1->jump();
    } else if (key == GameConfig::PLAYER1_CROUCH) {
        m_player1->crouch();
        // 尝试拾取物品
        for (auto& item : m_items) {
            if (item->isValid()) {
                Vector2D playerPos = m_player1->getPosition();
                Vector2D itemPos = item->getPosition();
                double distance = playerPos.distanceTo(itemPos);
                
                if (distance < 50) { // 拾取距离
                    if (m_player1->pickupItem(item)) {
                        break;
                    }
                }
            }
        }
    } else if (key == GameConfig::PLAYER1_FIRE) {
        handlePlayerAttack(m_player1);
    }
    
    // 玩家2控制
    if (key == GameConfig::PLAYER2_LEFT) {
        m_player2->moveLeft();
    } else if (key == GameConfig::PLAYER2_RIGHT) {
        m_player2->moveRight();
    } else if (key == GameConfig::PLAYER2_JUMP) {
        m_player2->jump();
    } else if (key == GameConfig::PLAYER2_CROUCH) {
        m_player2->crouch();
        // 尝试拾取物品
        for (auto& item : m_items) {
            if (item->isValid()) {
                Vector2D playerPos = m_player2->getPosition();
                Vector2D itemPos = item->getPosition();
                double distance = playerPos.distanceTo(itemPos);
                
                if (distance < 50) { // 拾取距离
                    if (m_player2->pickupItem(item)) {
                        break;
                    }
                }
            }
        }
    } else if (key == GameConfig::PLAYER2_FIRE) {
        handlePlayerAttack(m_player2);
    }
}

void GameEngine::handleKeyRelease(Qt::Key key) {
    if (m_gameState != GameState::PLAYING) return;
    
    // 玩家1控制
    if (key == GameConfig::PLAYER1_LEFT || key == GameConfig::PLAYER1_RIGHT) {
        m_player1->stopMoving();
    } else if (key == GameConfig::PLAYER1_CROUCH) {
        m_player1->stopCrouching();
    }
    
    // 玩家2控制
    if (key == GameConfig::PLAYER2_LEFT || key == GameConfig::PLAYER2_RIGHT) {
        m_player2->stopMoving();
    } else if (key == GameConfig::PLAYER2_CROUCH) {
        m_player2->stopCrouching();
    }
}

void GameEngine::spawnRandomItem() {
    if (m_gameState != GameState::PLAYING) return;
    
    ItemType itemType = generateRandomItemType();
    Vector2D dropPos = generateRandomDropPosition();
    
    std::shared_ptr<Item> item;
    
    switch (itemType) {
        case ItemType::WEAPON_KNIFE:
        case ItemType::WEAPON_BALL:
        case ItemType::WEAPON_RIFLE:
        case ItemType::WEAPON_SNIPER:
            item = std::make_shared<WeaponItem>(itemType, dropPos);
            break;
        case ItemType::BANDAGE:
            item = std::make_shared<BandageItem>(dropPos);
            break;
        case ItemType::MEDKIT:
            item = std::make_shared<MedkitItem>(dropPos);
            break;
        case ItemType::ADRENALINE:
            item = std::make_shared<AdrenalineItem>(dropPos);
            break;
    }
    
    if (item) {
        m_items.push_back(item);
    }
}

void GameEngine::createPlatforms() {
    m_platforms.clear();
    
    // 地面
    m_platforms.emplace_back(Vector2D(0, GameConfig::GROUND_LEVEL), 
                           GameConfig::WINDOW_WIDTH, 50, 
                           TerrainType::GROUND, QColor(139, 69, 19));
    
    // 中央平台（草地）
    m_platforms.emplace_back(Vector2D(450, 600), 
                           300, 20, 
                           TerrainType::GRASS, QColor(34, 139, 34));
    
    // 左侧平台（冰场）
    m_platforms.emplace_back(Vector2D(100, 500), 
                           200, 20, 
                           TerrainType::ICE, QColor(173, 216, 230));
    
    // 右侧平台（普通）
    m_platforms.emplace_back(Vector2D(900, 500), 
                           200, 20, 
                           TerrainType::GROUND, QColor(139, 69, 19));
    
    // 高层平台（冰场）
    m_platforms.emplace_back(Vector2D(350, 400), 
                           400, 20, 
                           TerrainType::ICE, QColor(173, 216, 230));
    
    // 左上角小平台（草地）
    m_platforms.emplace_back(Vector2D(50, 300), 
                           150, 20, 
                           TerrainType::GRASS, QColor(34, 139, 34));
    
    // 右上角小平台（草地）
    m_platforms.emplace_back(Vector2D(1000, 300), 
                           150, 20, 
                           TerrainType::GRASS, QColor(34, 139, 34));
}

void GameEngine::updatePhysics(double deltaTime) {
    // 检查玩家与平台碰撞
    checkPlayerPlatformCollision(m_player1);
    checkPlayerPlatformCollision(m_player2);
}

void GameEngine::updateProjectiles(double deltaTime) {
    // 更新投射物
    for (auto& projectile : m_projectiles) {
        projectile->update(deltaTime);
    }
    
    // 移除无效投射物
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                      [](const std::shared_ptr<Projectile>& p) {
                          return !p->isValid();
                      }),
        m_projectiles.end()
    );
}

void GameEngine::updateItems(double deltaTime) {
    // 更新物品
    for (auto& item : m_items) {
        item->update(deltaTime);
        
        // 检查物品与平台碰撞
        Vector2D itemPos = item->getPosition();
        Vector2D itemSize(item->getWidth(), item->getHeight());
        
        for (const auto& platform : m_platforms) {
            if (checkRectCollision(itemPos, itemSize, 
                                 platform.position, Vector2D(platform.width, platform.height))) {
                // 物品落在平台上
                item->setPosition(Vector2D(itemPos.x, platform.position.y - item->getHeight()));
                item->setVelocity(Vector2D(0, 0));
                item->setGrounded(true);
                break;
            }
        }
    }
    
    // 移除无效物品
    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
                      [](const std::shared_ptr<Item>& item) {
                          return !item->isValid();
                      }),
        m_items.end()
    );
}

void GameEngine::checkCollisions() {
    checkProjectilePlayerCollision();
    checkProjectilePlatformCollision();
    // 玩家与物品的碰撞在按键处理中检测
}

void GameEngine::checkPlayerPlatformCollision(std::shared_ptr<Player> player) {
    if (!player) return;
    
    Vector2D playerPos = player->getPosition();
    Vector2D playerSize(player->getWidth(), player->getHeight());
    Vector2D playerVel = player->getVelocity();
    
    bool wasGrounded = player->isGrounded();
    bool nowGrounded = false;
    
    for (const auto& platform : m_platforms) {
        Vector2D platformPos = platform.position;
        Vector2D platformSize(platform.width, platform.height);
        
        if (checkRectCollision(playerPos, playerSize, platformPos, platformSize)) {
            // 从上方落到平台上
            if (playerVel.y > 0 && playerPos.y < platformPos.y) {
                player->setPosition(Vector2D(playerPos.x, platformPos.y - playerSize.y));
                player->setVelocity(Vector2D(playerVel.x, 0));
                nowGrounded = true;
                player->setTerrainType(platform.type);
            }
            // 从下方撞到平台
            else if (playerVel.y < 0 && playerPos.y > platformPos.y) {
                player->setPosition(Vector2D(playerPos.x, platformPos.y + platformSize.y));
                player->setVelocity(Vector2D(playerVel.x, 0));
            }
            // 从侧面撞到平台
            else {
                if (playerVel.x > 0) {
                    player->setPosition(Vector2D(platformPos.x - playerSize.x, playerPos.y));
                } else if (playerVel.x < 0) {
                    player->setPosition(Vector2D(platformPos.x + platformSize.x, playerPos.y));
                }
                player->setVelocity(Vector2D(0, playerVel.y));
            }
        }
    }
    
    // 更新地面状态
    if (!nowGrounded && playerPos.y >= GameConfig::GROUND_LEVEL - playerSize.y) {
        nowGrounded = true;
    }
}

void GameEngine::checkProjectilePlayerCollision() {
    for (auto it = m_projectiles.begin(); it != m_projectiles.end();) {
        auto& projectile = *it;
        bool hit = false;
        
        // 检查与玩家1碰撞
        if (projectile->getOwnerId() != 0) { // 不是玩家1发射的
            Vector2D p1Pos = m_player1->getPosition();
            Vector2D p1Size(m_player1->getWidth(), m_player1->getHeight());
            
            if (!m_player1->isInvisible() && 
                checkCircleRectCollision(projectile->getPosition(), projectile->getRadius(), p1Pos, p1Size)) {
                m_player1->takeDamage(projectile->getDamage());
                hit = true;
            }
        }
        
        // 检查与玩家2碰撞
        if (!hit && projectile->getOwnerId() != 1) { // 不是玩家2发射的
            Vector2D p2Pos = m_player2->getPosition();
            Vector2D p2Size(m_player2->getWidth(), m_player2->getHeight());
            
            if (!m_player2->isInvisible() && 
                checkCircleRectCollision(projectile->getPosition(), projectile->getRadius(), p2Pos, p2Size)) {
                m_player2->takeDamage(projectile->getDamage());
                hit = true;
            }
        }
        
        if (hit) {
            it = m_projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void GameEngine::checkProjectilePlatformCollision() {
    for (auto it = m_projectiles.begin(); it != m_projectiles.end();) {
        auto& projectile = *it;
        bool hit = false;
        
        for (const auto& platform : m_platforms) {
            Vector2D platformPos = platform.position;
            Vector2D platformSize(platform.width, platform.height);
            
            if (checkCircleRectCollision(projectile->getPosition(), projectile->getRadius(), 
                                       platformPos, platformSize)) {
                hit = true;
                break;
            }
        }
        
        if (hit) {
            it = m_projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void GameEngine::handlePlayerAttack(std::shared_ptr<Player> player) {
    if (!player || !player->getWeapon()) return;
    
    player->attack();
    
    auto weapon = player->getWeapon();
    Vector2D targetPos = player->getPosition();
    
    // 近战武器直接伤害检测
    if (weapon->getType() == WeaponType::FIST || weapon->getType() == WeaponType::KNIFE) {
        if (!weapon->canAttack()) return;
        
        std::shared_ptr<Player> target = (player == m_player1) ? m_player2 : m_player1;
        Vector2D playerPos = player->getPosition();
        Vector2D targetPlayerPos = target->getPosition();
        
        double distance = playerPos.distanceTo(targetPlayerPos);
        double attackRange = weapon->getAttackRange();
        
        // 检查攻击方向
        bool facingTarget = (player->isFacingRight() && targetPlayerPos.x > playerPos.x) ||
                          (!player->isFacingRight() && targetPlayerPos.x < playerPos.x);
        
        if (distance <= attackRange && facingTarget && !target->isInvisible()) {
            target->takeDamage(weapon->getDamage());
        }
    }
    // 远程武器生成投射物
    else {
        auto projectile = weapon->attack(player.get(), targetPos);
        if (projectile) {
            m_projectiles.push_back(projectile);
        }
    }
}

bool GameEngine::checkRectCollision(const Vector2D& pos1, const Vector2D& size1,
                                   const Vector2D& pos2, const Vector2D& size2) {
    return pos1.x < pos2.x + size2.x &&
           pos1.x + size1.x > pos2.x &&
           pos1.y < pos2.y + size2.y &&
           pos1.y + size1.y > pos2.y;
}

bool GameEngine::checkCircleRectCollision(const Vector2D& circlePos, double radius,
                                         const Vector2D& rectPos, const Vector2D& rectSize) {
    // 找到矩形上离圆心最近的点
    double closestX = std::max(rectPos.x, std::min(circlePos.x, rectPos.x + rectSize.x));
    double closestY = std::max(rectPos.y, std::min(circlePos.y, rectPos.y + rectSize.y));
    
    // 计算距离
    double distance = Vector2D(circlePos.x - closestX, circlePos.y - closestY).length();
    
    return distance <= radius;
}

TerrainType GameEngine::getPlayerTerrainType(std::shared_ptr<Player> player) {
    Vector2D playerPos = player->getPosition();
    Vector2D playerSize(player->getWidth(), player->getHeight());
    
    for (const auto& platform : m_platforms) {
        Vector2D platformPos = platform.position;
        Vector2D platformSize(platform.width, platform.height);
        
        // 检查玩家是否站在平台上
        if (playerPos.x + playerSize.x > platformPos.x &&
            playerPos.x < platformPos.x + platformSize.x &&
            playerPos.y + playerSize.y >= platformPos.y &&
            playerPos.y + playerSize.y <= platformPos.y + platformSize.y + 10) {
            return platform.type;
        }
    }
    
    return TerrainType::GROUND;
}

ItemType GameEngine::generateRandomItemType() {
    std::uniform_int_distribution<int> dist(0, 6);
    int type = dist(m_randomGenerator);
    
    switch (type) {
        case 0: return ItemType::WEAPON_KNIFE;
        case 1: return ItemType::WEAPON_BALL;
        case 2: return ItemType::WEAPON_RIFLE;
        case 3: return ItemType::WEAPON_SNIPER;
        case 4: return ItemType::BANDAGE;
        case 5: return ItemType::MEDKIT;
        case 6: return ItemType::ADRENALINE;
        default: return ItemType::BANDAGE;
    }
}

Vector2D GameEngine::generateRandomDropPosition() {
    std::uniform_real_distribution<double> xDist(100, GameConfig::WINDOW_WIDTH - 100);
    double x = xDist(m_randomGenerator);
    double y = GameConfig::ITEM_DROP_HEIGHT;
    
    return Vector2D(x, y);
} 