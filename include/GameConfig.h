/**
 * @file GameConfig.h
 * @brief Game configuration constants definition
 * @author Justin0828
 * @date 2025-07-23
 */

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <Qt>

/**
 * @brief Game configuration class
 * 
 * Contains all game-related constant configurations
 */
class GameConfig {
public:
    // Window configuration
    static constexpr int WINDOW_WIDTH = 1200;     ///< Window width
    static constexpr int WINDOW_HEIGHT = 800;     ///< Window height
    static constexpr int TARGET_FPS = 60;         ///< Target frame rate

    // Physics configuration
    static constexpr double GRAVITY = 800.0;      ///< Gravity acceleration (pixels/second²)
    static constexpr double GROUND_LEVEL = 750.0; ///< Ground level height
    static constexpr double FRICTION = 0.8;       ///< Friction coefficient

    // Player configuration
    static constexpr double PLAYER_SPEED = 300.0;     ///< Player movement speed
    static constexpr double PLAYER_JUMP_SPEED = -500.0; ///< Player jump speed
    static constexpr int PLAYER_MAX_HP = 100;          ///< Player maximum health
    static constexpr double PLAYER_WIDTH = 40.0;       ///< Player width
    static constexpr double PLAYER_HEIGHT = 60.0;      ///< Player height

    // Terrain configuration
    static constexpr double ICE_SPEED_MULTIPLIER = 1.5; ///< Ice terrain speed multiplier
    
    // Weapon configuration
    static constexpr double BULLET_SPEED = 600.0;       ///< Bullet speed
    static constexpr double BALL_THROW_SPEED = 400.0;   ///< Ball throwing speed
    static constexpr double BALL_THROW_ANGLE = -30.0;   ///< Ball throwing angle (degrees)
    
    // Damage configuration
    static constexpr int FIST_DAMAGE = 15;        ///< Fist damage
    static constexpr int KNIFE_DAMAGE = 25;       ///< Knife damage
    static constexpr int BALL_DAMAGE = 35;        ///< Ball damage
    static constexpr int RIFLE_DAMAGE = 20;       ///< Rifle damage
    static constexpr int SNIPER_DAMAGE = 60;      ///< Sniper rifle damage
    
    // Weapon ammunition configuration
    static constexpr int RIFLE_AMMO = 30;         ///< Rifle ammunition
    static constexpr int SNIPER_AMMO = 5;         ///< Sniper rifle ammunition
    static constexpr int BALL_COUNT = 3;          ///< Ball count
    
    // Attack interval configuration (milliseconds)
    static constexpr int FIST_COOLDOWN = 800;     ///< Fist attack interval
    static constexpr int KNIFE_COOLDOWN = 600;    ///< Knife attack interval
    static constexpr int BALL_COOLDOWN = 1000;    ///< Ball throwing interval
    static constexpr int RIFLE_COOLDOWN = 150;    ///< Rifle shooting interval
    static constexpr int SNIPER_COOLDOWN = 2000;  ///< Sniper rifle shooting interval
    
    // Item healing amounts
    static constexpr int BANDAGE_HEAL = 20;       ///< Bandage healing amount
    static constexpr int MEDKIT_HEAL = 100;       ///< Medical kit healing amount
    static constexpr int ADRENALINE_HEAL = 5;     ///< Adrenaline healing per second
    static constexpr int ADRENALINE_DURATION = 10000; ///< Adrenaline duration (milliseconds)
    static constexpr double ADRENALINE_SPEED_MULTIPLIER = 1.3; ///< Adrenaline speed multiplier

    // Key mapping
    // Player 1
    static constexpr Qt::Key PLAYER1_LEFT = Qt::Key_A;   ///< Player 1 move left
    static constexpr Qt::Key PLAYER1_RIGHT = Qt::Key_D;  ///< Player 1 move right
    static constexpr Qt::Key PLAYER1_JUMP = Qt::Key_W;   ///< Player 1 jump
    static constexpr Qt::Key PLAYER1_CROUCH = Qt::Key_S; ///< Player 1 crouch/pickup
    static constexpr Qt::Key PLAYER1_FIRE = Qt::Key_Space; ///< Player 1 fire
    
    // Player 2
    static constexpr Qt::Key PLAYER2_LEFT = Qt::Key_J;   ///< Player 2 move left
    static constexpr Qt::Key PLAYER2_RIGHT = Qt::Key_L;  ///< Player 2 move right
    static constexpr Qt::Key PLAYER2_JUMP = Qt::Key_I;   ///< Player 2 jump
    static constexpr Qt::Key PLAYER2_CROUCH = Qt::Key_K; ///< Player 2 crouch/pickup
    static constexpr Qt::Key PLAYER2_FIRE = Qt::Key_Shift; ///< Player 2 fire

    // Item drop configuration
    static constexpr int ITEM_DROP_INTERVAL = 3000;     ///< Item drop interval (milliseconds)
    static constexpr double ITEM_DROP_HEIGHT = 50.0;    ///< Item drop height
};

#endif // GAMECONFIG_H 