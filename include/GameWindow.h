/**
 * @file GameWindow.h
 * @brief 游戏主窗口类定义
 * @author QtGame Team
 * @date 2024
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
 * @brief 游戏主窗口类
 * 
 * 继承自QMainWindow，负责游戏的渲染和用户界面
 */
class GameWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit GameWindow(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~GameWindow();

protected:
    /**
     * @brief 绘制事件
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief 按键按下事件
     * @param event 按键事件
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief 按键释放事件
     * @param event 按键事件
     */
    void keyReleaseEvent(QKeyEvent* event) override;

    /**
     * @brief 窗口关闭事件
     * @param event 关闭事件
     */
    void closeEvent(QCloseEvent* event) override;

private slots:
    /**
     * @brief 游戏循环更新
     */
    void gameLoop();

private:
    /**
     * @brief 初始化UI界面
     */
    void initializeUI();

    /**
     * @brief 绘制游戏元素
     * @param painter 绘图对象
     */
    void drawGame(QPainter* painter);

    /**
     * @brief 绘制背景
     * @param painter 绘图对象
     */
    void drawBackground(QPainter* painter);

    /**
     * @brief 绘制平台
     * @param painter 绘图对象
     */
    void drawPlatforms(QPainter* painter);

    /**
     * @brief 绘制玩家
     * @param painter 绘图对象
     */
    void drawPlayers(QPainter* painter);

    /**
     * @brief 绘制单个玩家
     * @param painter 绘图对象
     * @param player 玩家对象
     */
    void drawPlayer(QPainter* painter, std::shared_ptr<Player> player);

    /**
     * @brief 绘制投射物
     * @param painter 绘图对象
     */
    void drawProjectiles(QPainter* painter);

    /**
     * @brief 绘制物品
     * @param painter 绘图对象
     */
    void drawItems(QPainter* painter);

    /**
     * @brief 绘制UI界面
     * @param painter 绘图对象
     */
    void drawUI(QPainter* painter);

    /**
     * @brief 绘制生命值条
     * @param painter 绘图对象
     * @param player 玩家对象
     * @param x X坐标
     * @param y Y坐标
     * @param playerName 玩家名称
     */
    void drawHealthBar(QPainter* painter, std::shared_ptr<Player> player, 
                      int x, int y, const QString& playerName);

    /**
     * @brief 绘制武器信息
     * @param painter 绘图对象
     * @param player 玩家对象
     * @param x X坐标
     * @param y Y坐标
     */
    void drawWeaponInfo(QPainter* painter, std::shared_ptr<Player> player, int x, int y);

    /**
     * @brief 绘制游戏结束画面
     * @param painter 绘图对象
     */
    void drawGameOver(QPainter* painter);

    /**
     * @brief 绘制FPS信息
     * @param painter 绘图对象
     */
    void drawFPS(QPainter* painter);

    /**
     * @brief 更新FPS计算
     */
    void updateFPS();

    /**
     * @brief 获取武器类型名称
     * @param type 武器类型
     * @return QString 武器名称
     */
    QString getWeaponTypeName(WeaponType type);

    /**
     * @brief 获取地形颜色
     * @param type 地形类型
     * @return QColor 地形颜色
     */
    QColor getTerrainColor(TerrainType type);

private:
    std::unique_ptr<GameEngine> m_gameEngine; ///< 游戏引擎
    QTimer* m_gameTimer;                      ///< 游戏循环定时器
    
    // FPS计算
    std::chrono::high_resolution_clock::time_point m_lastFrameTime; ///< 上一帧时间
    int m_frameCount;                         ///< 帧计数
    double m_currentFPS;                      ///< 当前FPS
    std::chrono::high_resolution_clock::time_point m_fpsUpdateTime; ///< FPS更新时间

    // UI元素
    QWidget* m_centralWidget;                 ///< 中央窗口部件
    
    // 按键状态跟踪
    std::set<Qt::Key> m_pressedKeys;          ///< 当前按下的按键集合
};

#endif // GAMEWINDOW_H 