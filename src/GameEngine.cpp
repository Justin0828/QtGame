/**
 * @file GameEngine.cpp
 * @brief Game engine class implementation
 * @author Justin0828
 * @date 2025-07-23
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
    // Create players
    Vector2D player1StartPos(200, GameConfig::GROUND_LEVEL - GameConfig::PLAYER_HEIGHT);
    Vector2D player2StartPos(1000, GameConfig::GROUND_LEVEL - GameConfig::PLAYER_HEIGHT);
    
    m_player1 = std::make_shared<Player>(player1StartPos, QColor(0, 0, 255), true);  // Blue player 1
    m_player2 = std::make_shared<Player>(player2StartPos, QColor(255, 0, 0), false); // Red player 2
    
    // Create map
    createPlatforms();
    
    // Reset game state
    m_gameState = GameState::PLAYING;
    m_winner = 0;
    
    // Clear lists
    m_projectiles.clear();
    m_items.clear();
}

void GameEngine::startGame() {
    if (m_gameState != GameState::PLAYING) {
        m_gameState = GameState::PLAYING;
    }
    
    // Start item drop timer
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
    
    // Update players
    if (m_player1 && m_player1->isAlive()) {
        m_player1->update(deltaTime);
    }
    
    if (m_player2 && m_player2->isAlive()) {
        m_player2->update(deltaTime);
    }
    
    // Update physics system
    updatePhysics(deltaTime);
    
    // Update projectiles
    updateProjectiles(deltaTime);
    
    // Update items
    updateItems(deltaTime);
    
    // Check collisions
    checkCollisions();
    
    // Check game over conditions
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
    
    // Player 1 controls
    if (key == GameConfig::PLAYER1_LEFT) {
        m_player1->moveLeft();
    } else if (key == GameConfig::PLAYER1_RIGHT) {
        m_player1->moveRight();
    } else if (key == GameConfig::PLAYER1_JUMP) {
        m_player1->jump();
    } else if (key == GameConfig::PLAYER1_CROUCH) {
        m_player1->crouch();
        // Try to pick up items
        for (auto& item : m_items) {
            if (item->isValid()) {
                Vector2D playerPos = m_player1->getPosition();
                Vector2D playerSize(m_player1->getWidth(), m_player1->getHeight());
                Vector2D itemPos = item->getPosition();
                Vector2D itemSize(item->getWidth(), item->getHeight());
                
                // Expand pickup range
                Vector2D expandedPlayerPos(playerPos.x - 30, playerPos.y - 30);
                Vector2D expandedPlayerSize(playerSize.x + 60, playerSize.y + 60);
                
                if (checkRectCollision(expandedPlayerPos, expandedPlayerSize, itemPos, itemSize)) {
                    if (m_player1->pickupItem(item)) {
                        break;
                    }
                }
            }
        }
    } else if (key == GameConfig::PLAYER1_FIRE) {
        handlePlayerAttack(m_player1);
    }
    
    // Player 2 controls
    if (key == GameConfig::PLAYER2_LEFT) {
        m_player2->moveLeft();
    } else if (key == GameConfig::PLAYER2_RIGHT) {
        m_player2->moveRight();
    } else if (key == GameConfig::PLAYER2_JUMP) {
        m_player2->jump();
    } else if (key == GameConfig::PLAYER2_CROUCH) {
        m_player2->crouch();
        // Try to pick up items
        for (auto& item : m_items) {
            if (item->isValid()) {
                Vector2D playerPos = m_player2->getPosition();
                Vector2D playerSize(m_player2->getWidth(), m_player2->getHeight());
                Vector2D itemPos = item->getPosition();
                Vector2D itemSize(item->getWidth(), item->getHeight());
                
                // Expand pickup range
                Vector2D expandedPlayerPos(playerPos.x - 30, playerPos.y - 30);
                Vector2D expandedPlayerSize(playerSize.x + 60, playerSize.y + 60);
                
                if (checkRectCollision(expandedPlayerPos, expandedPlayerSize, itemPos, itemSize)) {
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
    
    // Player 1 controls
    if (key == GameConfig::PLAYER1_LEFT || key == GameConfig::PLAYER1_RIGHT) {
        m_player1->stopMoving();
    } else if (key == GameConfig::PLAYER1_CROUCH) {
        m_player1->stopCrouching();
    }
    
    // Player 2 controls
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
    
    // Ground
    m_platforms.emplace_back(Vector2D(0, GameConfig::GROUND_LEVEL), 
                           GameConfig::WINDOW_WIDTH, 50, 
                           TerrainType::GROUND, QColor(139, 69, 19));
    
    // Central platform (grass)
    m_platforms.emplace_back(Vector2D(450, 600), 
                           300, 20, 
                           TerrainType::GRASS, QColor(34, 139, 34));
    
    // Left platform (ice)
    m_platforms.emplace_back(Vector2D(100, 500), 
                           200, 20, 
                           TerrainType::ICE, QColor(173, 216, 230));
    
    // Right platform (normal)
    m_platforms.emplace_back(Vector2D(900, 500), 
                           200, 20, 
                           TerrainType::GROUND, QColor(139, 69, 19));
    
    // High-level platform (ice)
    m_platforms.emplace_back(Vector2D(350, 400), 
                           400, 20, 
                           TerrainType::ICE, QColor(173, 216, 230));
    
    // Top-left small platform (grass)
    m_platforms.emplace_back(Vector2D(50, 300), 
                           150, 20, 
                           TerrainType::GRASS, QColor(34, 139, 34));
    
    // Top-right small platform (grass)
    m_platforms.emplace_back(Vector2D(1000, 300), 
                           150, 20, 
                           TerrainType::GRASS, QColor(34, 139, 34));
}

void GameEngine::updatePhysics(double deltaTime) {
    // Check player-platform collision
    checkPlayerPlatformCollision(m_player1);
    checkPlayerPlatformCollision(m_player2);
}

void GameEngine::updateProjectiles(double deltaTime) {
    // Update projectiles
    for (auto& projectile : m_projectiles) {
        projectile->update(deltaTime);
    }
    
    // Remove invalid projectiles
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                      [](const std::shared_ptr<Projectile>& p) {
                          return !p->isValid();
                      }),
        m_projectiles.end()
    );
}

void GameEngine::updateItems(double deltaTime) {
    // Update items
    for (auto& item : m_items) {
        item->update(deltaTime);
        
        // Check item-platform collision
        Vector2D itemPos = item->getPosition();
        Vector2D itemSize(item->getWidth(), item->getHeight());
        
        for (const auto& platform : m_platforms) {
            if (checkRectCollision(itemPos, itemSize, 
                                 platform.position, Vector2D(platform.width, platform.height))) {
                // Item landed on platform
                item->setPosition(Vector2D(itemPos.x, platform.position.y - item->getHeight()));
                item->setVelocity(Vector2D(0, 0));
                item->setGrounded(true);
                break;
            }
        }
    }
    
    // Remove invalid items
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
    // Player-item collision is checked in key handling
}

void GameEngine::checkPlayerPlatformCollision(std::shared_ptr<Player> player) {
    if (!player) return;
    
    Vector2D playerPos = player->getPosition();
    Vector2D playerSize(player->getWidth(), player->getHeight());
    Vector2D playerVel = player->getVelocity();
    
    bool nowGrounded = false;
    
    for (const auto& platform : m_platforms) {
        Vector2D platformPos = platform.position;
        Vector2D platformSize(platform.width, platform.height);
        
        if (checkRectCollision(playerPos, playerSize, platformPos, platformSize)) {
            // Landing on platform from above
            if (playerVel.y > 0 && playerPos.y < platformPos.y) {
                player->setPosition(Vector2D(playerPos.x, platformPos.y - playerSize.y));
                player->setVelocity(Vector2D(playerVel.x, 0));
                nowGrounded = true;
            }
            // Hitting platform from below
            else if (playerVel.y < 0 && playerPos.y > platformPos.y) {
                player->setPosition(Vector2D(playerPos.x, platformPos.y + platformSize.y));
                player->setVelocity(Vector2D(playerVel.x, 0));
            }
            // Hitting platform from side
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
    
    // Check if player is standing on any platform (including ground)
    if (!nowGrounded) {
        // Check if on ground
        if (playerPos.y + playerSize.y >= GameConfig::GROUND_LEVEL - 5) {
            nowGrounded = true;
        } else {
            // Check if standing on platform
            for (const auto& platform : m_platforms) {
                Vector2D platformPos = platform.position;
                Vector2D platformSize(platform.width, platform.height);
                
                // Check horizontal overlap
                if (playerPos.x + playerSize.x > platformPos.x && 
                    playerPos.x < platformPos.x + platformSize.x) {
                    // Check if standing on platform top
                    if (playerPos.y + playerSize.y >= platformPos.y && 
                        playerPos.y + playerSize.y <= platformPos.y + 10) {
                        nowGrounded = true;
                        break;
                    }
                }
            }
        }
    }
    
    // Update player's ground state
    if (nowGrounded) {
        if (!player->isGrounded()) {
            // Just landed, reset vertical velocity
            if (playerVel.y > 0) {
                player->setVelocity(Vector2D(playerVel.x, 0));
            }
        }
        // Force set ground state
        player->setGrounded(true);
        
        // Update terrain type (after setting ground state)
        TerrainType currentTerrain = getPlayerTerrainType(player);
        player->setTerrainType(currentTerrain);
    } else {
        player->setGrounded(false);
        // Set to normal terrain when airborne
        player->setTerrainType(TerrainType::GROUND);
    }
}

void GameEngine::checkProjectilePlayerCollision() {
    for (auto it = m_projectiles.begin(); it != m_projectiles.end();) {
        auto& projectile = *it;
        bool hit = false;
        
        // Check collision with player 1
        if (projectile->getOwnerId() != 0) { // Not fired by player 1
            Vector2D p1Pos = m_player1->getPosition();
            Vector2D p1Size(m_player1->getWidth(), m_player1->getHeight());
            
            if (!m_player1->isInvisible() && 
                checkCircleRectCollision(projectile->getPosition(), projectile->getRadius(), p1Pos, p1Size)) {
                m_player1->takeDamage(projectile->getDamage());
                hit = true;
            }
        }
        
        // Check collision with player 2
        if (!hit && projectile->getOwnerId() != 1) { // Not fired by player 2
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
    
    // Melee weapon direct damage detection
    if (weapon->getType() == WeaponType::FIST || weapon->getType() == WeaponType::KNIFE) {
        if (!weapon->canAttack()) return;
        
        std::shared_ptr<Player> target = (player == m_player1) ? m_player2 : m_player1;
        Vector2D playerPos = player->getPosition();
        Vector2D targetPlayerPos = target->getPosition();
        
        double distance = playerPos.distanceTo(targetPlayerPos);
        double attackRange = weapon->getAttackRange();
        
        // Check attack direction
        bool facingTarget = (player->isFacingRight() && targetPlayerPos.x > playerPos.x) ||
                          (!player->isFacingRight() && targetPlayerPos.x < playerPos.x);
        
        if (distance <= attackRange && facingTarget && !target->isInvisible()) {
            target->takeDamage(weapon->getDamage());
        }
    }
    // Ranged weapon generate projectiles
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
    // Find the closest point on rectangle to circle center
    double closestX = std::max(rectPos.x, std::min(circlePos.x, rectPos.x + rectSize.x));
    double closestY = std::max(rectPos.y, std::min(circlePos.y, rectPos.y + rectSize.y));
    
    // Calculate distance
    double distance = Vector2D(circlePos.x - closestX, circlePos.y - closestY).length();
    
    return distance <= radius;
}

TerrainType GameEngine::getPlayerTerrainType(std::shared_ptr<Player> player) {
    Vector2D playerPos = player->getPosition();
    Vector2D playerSize(player->getWidth(), player->getHeight());
    
    // Only detect terrain type for grounded players
    if (!player->isGrounded()) {
        return TerrainType::GROUND;
    }
    
    for (const auto& platform : m_platforms) {
        Vector2D platformPos = platform.position;
        Vector2D platformSize(platform.width, platform.height);
        
        // Check if player is within platform's horizontal range
        if (playerPos.x + playerSize.x > platformPos.x &&
            playerPos.x < platformPos.x + platformSize.x) {
            // Check if player is standing on platform top (with tolerance)
            double playerBottom = playerPos.y + playerSize.y;
            if (playerBottom >= platformPos.y - 5 && 
                playerBottom <= platformPos.y + platformSize.y + 15) {
                return platform.type;
            }
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