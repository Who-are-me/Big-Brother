#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QString>



int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(resource);

    QApplication application(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow mainwindow;
    mainwindow.show();

    QFile style_file(":/qss/resource/style.qss");
    style_file.open(QFile::ReadOnly);
    QString style(style_file.readAll());

    application.setStyleSheet(style);

    return application.exec();
}
