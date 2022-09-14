#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "window.hpp"

SpotifyFrame::SpotifyFrame()
    : mpris("org.mpris.MediaPlayer2.spotify",
            "/org/mpris/MediaPlayer2",
            "org.mpris.MediaPlayer2.Player",
            QDBusConnection::sessionBus()) {
    setWindowTitle(tr("Spotify"));

    setWindowIcon(QIcon(":/icons/spotify-client.png"));

    trayIcon = new QSystemTrayIcon(QIcon(":/icons/spotify-intray.png"), this);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &SpotifyFrame::iconActivated);

    quitAction = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit Spotify"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    prevAction = new QAction(QIcon::fromTheme("media-skip-backward"), tr("Pre&vious Track"), this);
    connect(prevAction, &QAction::triggered, this, &SpotifyFrame::prev);

    nextAction = new QAction(QIcon::fromTheme("media-skip-forward"), tr("&Next Track"), this);
    connect(nextAction, &QAction::triggered, this, &SpotifyFrame::next);

    playAction = new QAction(QIcon::fromTheme("media-playback-start"), tr("P&lay"), this);
    connect(playAction, &QAction::triggered, this, &SpotifyFrame::play);

    pauseAction = new QAction(QIcon::fromTheme("media-playback-pause"), tr("&Pause"), this);
    pauseAction->setVisible(false);
    connect(pauseAction, &QAction::triggered, this, &SpotifyFrame::pause);

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

void SpotifyFrame::activate() {
    show();
    raise();
    activateWindow();
}

void SpotifyFrame::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if (isVisible())
                hide();
            else
                activate();
            break;
        case QSystemTrayIcon::MiddleClick: break;
        default:;
    }
}

void SpotifyFrame::closeEvent(QCloseEvent* event) {
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void SpotifyFrame::prev() { mpris.call("Previous"); }

void SpotifyFrame::next() { mpris.call("Next"); }

void SpotifyFrame::play() { mpris.call("Play"); }

void SpotifyFrame::pause() { mpris.call("Pause"); }

void SpotifyFrame::dbusPropertiesChanged(const QString& name, const QVariantMap& properties, const QStringList&) {
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
