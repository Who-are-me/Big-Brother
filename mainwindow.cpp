#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGraphicsScene>
#include <QListWidgetItem>
#include <QDebug>
#include <QCryptographicHash>
#include <QByteArray>
#include <thread>
#include <chrono>
#include <algorithm>


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
    delete this->query_model;

    if(!this->temp_data.empty()) {
        for(QVariantList x : temp_data) {
            this->database->insertIntoTable(x);
        }
        this->temp_data.clear();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::startStopScreenshots() {
    qDebug() << "MainWindow::startStopScreenshots clicked\n";

    this->startStopWork();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_pb_startstop_released() {
    qDebug() << "PushBatton: pb_startstop released\n";

    this->startStopWork();
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
    this->query_model = new QSqlQueryModel;

    this->query_model->setQuery("SELECT COUNT(*) FROM Data");
    int count_of_rows = this->query_model->data(this->query_model->index(0, 0)).toInt();

    this->query_model->setQuery("SELECT * FROM Data");

    QPixmap pm;
    QByteArray ba;
    QString hash;

    if(count_of_rows == 0) {
        this->last_screenshot = QByteArray::fromStdString("empty empty empty");
    }

    for(int x = 0; x < count_of_rows; ++x) {
        ba = this->query_model->data(this->query_model->index(x, 1)).toByteArray();
        hash = this->query_model->data(this->query_model->index(x, 2)).toString();
        pm.loadFromData(ba, "PNG" );

        if(count_of_rows-1 == x) {
            this->last_screenshot = ba;
        }

        ui->listOfImages->insertItem(0, new QListWidgetItem(pm, hash));
    }

    qDebug() << "MainWindow::configurateDatabase done\n";
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::configurateWindow() {
    ui->statusbar->showMessage("");
    QPalette p;
    ui->listOfImages->setViewMode(QListWidget::IconMode);
    this->ui->listOfImages->setIconSize(QSize(200, 200));
    this->ui->listOfImages->setResizeMode(QListWidget::Adjust);
    p.setBrush(QPalette::Window, Qt::transparent);
    this->ui->listOfImages->setPalette(p);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

QPixmap MainWindow::getScreenshot() {
    QScreen *screen = QGuiApplication::primaryScreen();

    if (const QWindow *window = windowHandle())
        screen = window->screen();

    // beep ! ! !
    QApplication::beep();

    QPixmap screenshot = screen->grabWindow(0);
    QByteArray ba;
    QBuffer buffer(&ba);

    screenshot.save(&buffer, "PNG");

    this->last_screenshot = ba;

    return screen->grabWindow(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::startStopWork() {
    this->is_work = !this->is_work;

    if(is_work) {
        this->workInThread();
    }

    if(!this->temp_data.empty()) {
        for(QVariantList x : temp_data) {
            this->database->insertIntoTable(x);
        }
        this->temp_data.clear();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::workInThread() {
    std::thread second_thread(&MainWindow::worker, this);
    second_thread.detach();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::worker() {
    qDebug() << "MainWindow::worker is run\n";

    while(this->is_work) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ui->sb_time->text().toInt() * 1000));

        if(!this->is_work)
            break;

        QPixmap screenshot = this->getScreenshot();
        QByteArray ba;
        QBuffer buffer(&ba);

        screenshot.save(&buffer, "PNG");

        QByteArray image_file = ba;
        QByteArray hash_image = QCryptographicHash::hash(image_file, QCryptographicHash::Sha1);
        QString hash_of_image = QString::fromStdString(hash_image.toHex().toStdString());

        qDebug() << "Hash of images: " << hash_of_image << "\n";

        this->data.clear();
        this->data.append(image_file);
        this->data.append(hash_of_image);

        double  similarity_charters = 0,
                similarity_percent = 0;


        for(size_t it = 0; it < std::min(this->last_screenshot.size(), image_file.size()); ++it) {
            if(this->last_screenshot.at(it) == image_file.at(it)) {
                ++similarity_charters;
            }
        }

        similarity_percent = (similarity_charters / std::min(this->last_screenshot.size(), image_file.size()) * 100);

        qDebug() << "Similarity of images: " << similarity_percent << "\n";

        this->data.append(similarity_percent);
        this->temp_data.append(this->data);

        ui->persent_of_similarity->setText(QString::number(similarity_percent));
        ui->statusbar->showMessage("Hash sum: " + hash_of_image);
        ui->listOfImages->insertItem(0, new QListWidgetItem(screenshot, hash_of_image));
    }
}

