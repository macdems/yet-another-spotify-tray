#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "mainwindow.hpp"

MainWindow::MainWindow()
    : dbus("org.mpris.MediaPlayer2.spotify",
           "/org/mpris/MediaPlayer2",
           "org.mpris.MediaPlayer2.Player",
           QDBusConnection::sessionBus()) {
    setWindowTitle(tr("Spotify"));

    trayIcon = new QSystemTrayIcon(QIcon::fromTheme("spotify", QIcon("/usr/share/pixmaps/spotify-client.png")), this);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    quitAction = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit Spotify"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    prevAction = new QAction(QIcon::fromTheme("media-skip-backward"), tr("Pre&vious Track"), this);
    connect(prevAction, &QAction::triggered, this, &MainWindow::prev);

    nextAction = new QAction(QIcon::fromTheme("media-skip-forward"), tr("&Next Track"), this);
    connect(nextAction, &QAction::triggered, this, &MainWindow::next);

    playAction = new QAction(QIcon::fromTheme("media-playback-start"), tr("&Play"), this);
    connect(playAction, &QAction::triggered, this, &MainWindow::play);

    pauseAction = new QAction(QIcon::fromTheme("media-playback-pause"), tr("&Pause"), this);
    connect(pauseAction, &QAction::triggered, this, &MainWindow::pause);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(prevAction);
    trayIconMenu->addAction(playAction);
    trayIconMenu->addAction(pauseAction);
    trayIconMenu->addAction(nextAction);

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon->setToolTip(tr("Spotify"));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if (isVisible()) {
                hide();
            } else {
                show();
                raise();
                activateWindow();
            }
            break;
        case QSystemTrayIcon::MiddleClick: break;
        default:;
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void MainWindow::prev() { dbus.call("Previous"); }

void MainWindow::next() { dbus.call("Next"); }

void MainWindow::play() { dbus.call("Play"); }

void MainWindow::pause() { dbus.call("Pause"); }
