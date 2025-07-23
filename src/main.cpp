/**
 * @file main.cpp
 * @brief Application entry point
 * @author Justin0828
 * @date 2025-07-23
 */

#include "GameWindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Configure application information
    app.setApplicationName("QtGame");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("QtGame Team");
    
    // Set default font
    QFont font("Arial", 10);
    app.setFont(font);
    
    // Create and display main window
    GameWindow window;
    window.show();
    
    return app.exec();
} 