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

    playAction = new QAction(QIcon::fromTheme("media-playback-start"), tr("P&lay"), this);
    connect(playAction, &QAction::triggered, this, &MainWindow::play);

    pauseAction = new QAction(QIcon::fromTheme("media-playback-pause"), tr("&Pause"), this);
    pauseAction->setVisible(false);
    connect(pauseAction, &QAction::triggered, this, &MainWindow::pause);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(prevAction);
    trayIconMenu->addAction(playAction);
    trayIconMenu->addAction(pauseAction);
    trayIconMenu->addAction(nextAction);

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    QDBusConnection::sessionBus().connect(nullptr, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged", this,
                                          SLOT(dbusPropertiesChanged(QString, QVariantMap, QStringList)));

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

void MainWindow::dbusPropertiesChanged(const QString& name, const QVariantMap& properties, const QStringList&) {
    if (name == "org.mpris.MediaPlayer2.Player") {
        if (properties.contains("PlaybackStatus")) {
            QString status = properties["PlaybackStatus"].toString();
            if (status == "Playing") {
                playAction->setVisible(false);
                pauseAction->setVisible(true);
            } else {
                playAction->setVisible(true);
                pauseAction->setVisible(false);
            }
        }

        if (properties.contains("Metadata")) {
            QVariantMap metadata;

            if (properties["Metadata"].type() == QVariant::Map) {
                metadata = properties["Metadata"].toMap();
            } else {
                properties["Metadata"].value<QDBusArgument>() >> metadata;
            }

            // for (QVariantMap::const_iterator it = metadata.cbegin(), end = metadata.cend(); it != end; ++it) {
            //     qDebug() << "metadata: " << it.key() << " value: " << it.value();
            // }

            QString title = metadata["xesam:title"].toString();
            QString artist = metadata["xesam:artist"].toStringList().join(", ");
            // QString album = metadata["xesam:album"].toString();

            if (title.isEmpty()) {
                title = artist;
                artist = "";
            }

            if (!title.isEmpty()) {
                if (!artist.isEmpty())
                    trayIcon->setToolTip(tr("Spotify - %1 - %2").arg(title).arg(artist));
                else
                    trayIcon->setToolTip(tr("Spotify - %1").arg(title));
            } else {
                trayIcon->setToolTip(tr("Spotify"));
            }

            // QString cover = metadata["mpris:artUrl"].toString();
            // if (!cover.isEmpty()) {
            //     QPixmap pixmap;
            //     pixmap.loadFromData(QByteArray::fromBase64(cover.toUtf8()));
            //     trayIcon->setIcon(pixmap);
            // } else {
            //     trayIcon->setIcon(QIcon::fromTheme("spotify", QIcon("/usr/share/pixmaps/spotify-client.png")));
            // }
        }
    }
}
