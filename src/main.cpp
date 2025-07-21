/**
 * @file main.cpp
 * @brief 应用程序入口点
 * @author QtGame Team
 * @date 2024
 */

#include "GameWindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("QtGame");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("QtGame Team");
    
    // 设置默认字体
    QFont font("Arial", 10);
    app.setFont(font);
    
    // 创建并显示主窗口
    GameWindow window;
    window.show();
    
    return app.exec();
} 