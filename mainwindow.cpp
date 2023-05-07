#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsScene>
#include <QListWidgetItem>
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>
#include <thread>
#include <chrono>


//////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "MainWindow: configurate starts\n";

    this->configurateSignalsSlots();
    this->configurateSystemTray();
    this->configurateDatabase();
    this->configurateWindow();
    this->is_work = false;


    qDebug() << "MainWindow: configurate done\n";



    // add to image grid
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/eye.png"), "eye"));
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/whiteeye.png"), "whiteeye"));
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/whiteeye.png"), "whiteeye"));
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/whiteeye.png"), "whiteeye"));
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/whiteeye.png"), "whiteeye"));
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/whiteeye.png"), "whiteeye"));
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/whiteeye.png"), "whiteeye"));
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/whiteeye.png"), "whiteeye"));
//    ui->listOfImages->addItem(new QListWidgetItem(QIcon(":/resource/whiteeye.png"), "whiteeye"));
}

//////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow() {
    delete ui;
    delete this->database;
    delete this->startStopAction;
    delete this->minimizeAction;
    delete this->maximizeAction;
    delete this->restoreAction;
    delete this->quitAction;
    delete this->trayIcon;
    delete this->trayIconMenu;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::startStopScreenshots() {
    qDebug() << "MainWindow::startStopScreenshots clicked\n";

    ui->statusbar->showMessage("Hash sum: ---");
    ui->listOfImages->scrollToBottom();

    this->workInThread();


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    QImage im = originalPixmap.toImage();
//    QByteArray ba;
//    QBuffer buffer(&ba);
//    im.save(&buffer, "PNG");

//    QByteArray image_file = ba;
//    QByteArray hash_image = QCryptographicHash::hash(image_file, QCryptographicHash::Sha1);

//    qDebug() << QString::fromStdString(hash_image.toHex().toStdString()) << "\n";

//    ui->statusbar->showMessage(QString::fromStdString(hash_image.toHex().toStdString()));
//    ui->listOfImages->addItem(new QListWidgetItem(originalPixmap, "eye"));
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::configurateSignalsSlots() {
    startStopAction = new QAction(tr("S&tart/Stop screenshots"), this);
    connect(startStopAction, &QAction::triggered, this, &MainWindow::startStopScreenshots);

    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    qDebug() << "MainWindow::configurateSignalsSlots done\n";
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::configurateSystemTray() {
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(startStopAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon(":/resource/whiteeye.png");
    trayIcon->setIcon(icon);

    trayIcon->show();

    qDebug() << "MainWindow::configurateSystemTray done\n";
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::configurateDatabase() {
    this->database = new Database;
    this->database->connectToDatabase();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::configurateWindow() {
    ui->statusbar->showMessage("Hash sum: ---");
    QPalette p;
    ui->listOfImages->setViewMode(QListWidget::IconMode);
    this->ui->listOfImages->setIconSize(QSize(200, 200));
    this->ui->listOfImages->setResizeMode(QListWidget::Adjust);
    p.setBrush(QPalette::Window, Qt::transparent);
    this->ui->listOfImages->setPalette(p);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

QPixmap& MainWindow::getScreenshot() {
    QPixmap screenshot;
    QScreen *screen = QGuiApplication::primaryScreen();

    if (const QWindow *window = windowHandle())
        screen = window->screen();

    // beep ! ! !
    QApplication::beep();

    screenshot = screen->grabWindow(0);

    return screenshot;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::workInThread() {
    std::thread second_thread(&MainWindow::worker, this);
    second_thread.detach();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::worker() {
    qDebug() << "MainWindow::worker is run\n";
    // simulation hard work
//    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

