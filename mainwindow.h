#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSqlQueryModel>
#include <QtGui>

#include "database.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QMenu;
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startStopScreenshots();

private:
    Ui::MainWindow 	*ui;
    Database 		*database;
    QSqlQueryModel 	*query_model;
    bool		is_work;

    QAction *startStopAction;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon 	*trayIcon;
    QMenu 				*trayIconMenu;

    void configurateSignalsSlots();
    void configurateSystemTray();
    void configurateDatabase();
    void configurateWindow();

    QPixmap& getScreenshot();
    void workInThread();
    void worker();
};
