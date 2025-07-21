/**
 * @file GameConfig.h
 * @brief 游戏配置常量定义
 * @author QtGame Team
 * @date 2024
 */

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <Qt>

/**
 * @brief 游戏配置类
 * 
 * 包含所有游戏相关的常量配置
 */
class GameConfig {
public:
    // 窗口配置
    static constexpr int WINDOW_WIDTH = 1200;     ///< 窗口宽度
    static constexpr int WINDOW_HEIGHT = 800;     ///< 窗口高度
    static constexpr int TARGET_FPS = 60;         ///< 目标帧率

    // 物理配置
    static constexpr double GRAVITY = 800.0;      ///< 重力加速度 (像素/秒²)
    static constexpr double GROUND_LEVEL = 750.0; ///< 地面高度
    static constexpr double FRICTION = 0.8;       ///< 摩擦系数

    // 玩家配置
    static constexpr double PLAYER_SPEED = 300.0;     ///< 玩家移动速度
    static constexpr double PLAYER_JUMP_SPEED = -500.0; ///< 玩家跳跃速度
    static constexpr int PLAYER_MAX_HP = 100;          ///< 玩家最大生命值
    static constexpr double PLAYER_WIDTH = 40.0;       ///< 玩家宽度
    static constexpr double PLAYER_HEIGHT = 60.0;      ///< 玩家高度

    // 地形配置
    static constexpr double ICE_SPEED_MULTIPLIER = 1.5; ///< 冰面速度倍数
    
    // 武器配置
    static constexpr double BULLET_SPEED = 600.0;       ///< 子弹速度
    static constexpr double BALL_THROW_SPEED = 400.0;   ///< 实心球投掷速度
    static constexpr double BALL_THROW_ANGLE = -30.0;   ///< 实心球投掷角度(度)
    
    // 伤害配置
    static constexpr int FIST_DAMAGE = 15;        ///< 拳头伤害
    static constexpr int KNIFE_DAMAGE = 25;       ///< 小刀伤害
    static constexpr int BALL_DAMAGE = 35;        ///< 实心球伤害
    static constexpr int RIFLE_DAMAGE = 20;       ///< 步枪伤害
    static constexpr int SNIPER_DAMAGE = 60;      ///< 狙击枪伤害
    
    // 武器弹药配置
    static constexpr int RIFLE_AMMO = 30;         ///< 步枪弹药
    static constexpr int SNIPER_AMMO = 5;         ///< 狙击枪弹药
    static constexpr int BALL_COUNT = 3;          ///< 实心球数量
    
    // 攻击间隔配置 (毫秒)
    static constexpr int FIST_COOLDOWN = 800;     ///< 拳头攻击间隔
    static constexpr int KNIFE_COOLDOWN = 600;    ///< 小刀攻击间隔
    static constexpr int BALL_COOLDOWN = 1000;    ///< 实心球投掷间隔
    static constexpr int RIFLE_COOLDOWN = 150;    ///< 步枪射击间隔
    static constexpr int SNIPER_COOLDOWN = 2000;  ///< 狙击枪射击间隔
    
    // 物品恢复量
    static constexpr int BANDAGE_HEAL = 20;       ///< 绷带恢复量
    static constexpr int MEDKIT_HEAL = 100;       ///< 医疗箱恢复量
    static constexpr int ADRENALINE_HEAL = 5;     ///< 肾上腺素每秒恢复量
    static constexpr int ADRENALINE_DURATION = 10000; ///< 肾上腺素持续时间(毫秒)
    static constexpr double ADRENALINE_SPEED_MULTIPLIER = 1.3; ///< 肾上腺素速度倍数

    // 按键映射
    // Player 1
    static constexpr Qt::Key PLAYER1_LEFT = Qt::Key_A;   ///< 玩家1左移
    static constexpr Qt::Key PLAYER1_RIGHT = Qt::Key_D;  ///< 玩家1右移
    static constexpr Qt::Key PLAYER1_JUMP = Qt::Key_W;   ///< 玩家1跳跃
    static constexpr Qt::Key PLAYER1_CROUCH = Qt::Key_S; ///< 玩家1下蹲/拾取
    static constexpr Qt::Key PLAYER1_FIRE = Qt::Key_Space; ///< 玩家1开火
    
    // Player 2
    static constexpr Qt::Key PLAYER2_LEFT = Qt::Key_J;   ///< 玩家2左移
    static constexpr Qt::Key PLAYER2_RIGHT = Qt::Key_L;  ///< 玩家2右移
    static constexpr Qt::Key PLAYER2_JUMP = Qt::Key_I;   ///< 玩家2跳跃
    static constexpr Qt::Key PLAYER2_CROUCH = Qt::Key_K; ///< 玩家2下蹲/拾取
    static constexpr Qt::Key PLAYER2_FIRE = Qt::Key_Shift; ///< 玩家2开火

    // 物品掉落配置
    static constexpr int ITEM_DROP_INTERVAL = 3000;     ///< 物品掉落间隔(毫秒)
    static constexpr double ITEM_DROP_HEIGHT = 50.0;    ///< 物品掉落高度
};

#endif // GAMECONFIG_H 