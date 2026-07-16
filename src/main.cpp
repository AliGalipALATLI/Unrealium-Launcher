#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    qputenv("QT_QPA_PLATFORM", "wayland;xcb"); // Wayland öncelikli, XWayland fallback
    
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Unrealium Launcher");

    qApp->setStyleSheet(R"(
        QWidget        { background: #1e1e1e; color: #ffffff; }
        QPushButton    { background: #2d2d2d; border: 1px solid #444;
                         border-radius: 6px; padding: 8px 18px; color: #ffffff; }
        QPushButton:hover   { background: #3a3a3a; }
        QPushButton:pressed { background: #252525; }
        QLineEdit      { background: #2d2d2d; border: 1px solid #444;
                         border-radius: 4px; padding: 4px 8px; color: #ffffff; }
        QLabel         { color: #ffffff; }
        QDialog        { background: #1e1e1e; color: #ffffff; }
        QMenu          { background: #2d2d2d; color: #ffffff; border: 1px solid #444; }
        QMenu::item:selected { background: #3a3a3a; }
        QScrollArea    { border: none; }
    )");

    MainWindow w;
    w.show();
    return a.exec();
}
