#include <unistd.h>

#include <QtWidgets>
#include <stdexcept>

#include "mainwindow.hpp"
#include "winctl.hpp"

#define DEFAULT_CLIENT_APP_PATH "spotify"
#define CLIENT_FIND_ATTEMPTS 5

#include <iostream>

// Spotify process handle
QProcess spotifyProcess;

// Try to get Spotify window, trying to
// spawn a new process using the args if the window is not found
// at the first attempt.
WindowData startSpotify(const QStringList& args) {
    spotifyProcess.start(DEFAULT_CLIENT_APP_PATH, args, QIODevice::ReadOnly);

    // App launched, it double forks; need to find the window and PID
    for (int i = 0; i < CLIENT_FIND_ATTEMPTS; i++) {
        // Spotify takes time to start up, so wait a while.
        usleep(5e5);  // 0.5 sec
        if (WindowData found = getSpotifyWindow()) {
            return found;
        } else {
            // Still nothing, try again.
            qWarning(
                "Could not find the Spotify client window: "
                "attempting to launch the application, attempt %d/%d",
                i + 1, CLIENT_FIND_ATTEMPTS);
        }
    }
    return WindowData();
}

int main(int argc, char** argv) {
    if (getSpotifyWindow()) {
        qCritical("Spotify is already running");
        return 1001;
    }

    QApplication app(argc, argv);

    QTranslator translator;
    if (translator.load(QLocale::system().name(), ":/translations")) app.installTranslator(&translator);

    MainWindow* main_window = new MainWindow();

    WindowData spotify = startSpotify(app.arguments());
    if (!spotify) return 1002;
    qDebug("Spotify PID: %lu", spotify.pid);

    QWindow* spotify_window = QWindow::fromWinId(spotify.wid);

    XWindowAttributes attr;
    XGetWindowAttributes(XOpenDisplay(nullptr), spotify.wid, &attr);
    main_window->resize(attr.width, attr.height);
    main_window->move(attr.x, attr.y);

    QWidget* widget = QWidget::createWindowContainer(spotify_window, main_window, Qt::FramelessWindowHint);
    main_window->setCentralWidget(widget);

    int exitcode = app.exec();

    delete main_window;
    return exitcode;
}
