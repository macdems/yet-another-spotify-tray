#include <unistd.h>
#include <signal.h>

#include "app.hpp"

SpotifyTrayApp::SpotifyTrayApp(int &argc, char **argv): QApplication(argc, argv) {
    setOrganizationName("macdems");
    setApplicationName("spotify-tray");

    bool show_window = true;

    spotifyArgs = arguments();
    spotifyArgs.takeFirst();
    // This is not proper argument parsing, as in general arguments are passed to Spotify
    while (!spotifyArgs.empty()) {
        if (spotifyArgs.first() == "--hide" || spotifyArgs.first() == "--hide-window") {
            spotifyArgs.takeFirst();
            show_window = false;
        } else if (spotifyArgs.first() == "--show" || spotifyArgs.first() == "--show-window") {
            spotifyArgs.takeFirst();
            show_window = true;
        } else {
            if (spotifyArgs.first() == "--") spotifyArgs.takeFirst();
            break;
        }
    }

    QTranslator translator;
    if (translator.load(QLocale::system().name(), ":/translations")) installTranslator(&translator);

    mainFrame = new SpotifyFrame();

    if (spotify = getSpotifyWindow()) {
        qDebug("Spotify is already running (PID %lu)", spotify.pid);
    } else {
        spotify = startSpotify(spotifyArgs);
        if (!spotify) throw 2;
        qDebug("Starting Spotify (PID %lu)", spotify.pid);
    }

    QWindow* spotify_window = QWindow::fromWinId(spotify.wid);

    XWindowAttributes attr;
    XGetWindowAttributes(XOpenDisplay(nullptr), spotify.wid, &attr);
    mainFrame->resize(attr.width, attr.height);
    mainFrame->move(attr.x, attr.y);

    QWidget* widget = QWidget::createWindowContainer(spotify_window, mainFrame, Qt::FramelessWindowHint);
    mainFrame->setCentralWidget(widget);

    if (show_window) mainFrame->show();
}

SpotifyTrayApp::~SpotifyTrayApp() {
    stopSpotify();
    delete mainFrame;
}

// Try to get Spotify window, trying to
// spawn a new process using the args if the window is not found
// at the first attempt.
WindowData SpotifyTrayApp::startSpotify(const QStringList& args) {
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

void SpotifyTrayApp::stopSpotify() {
    if (spotifyProcess.state() != QProcess::NotRunning)
        spotifyProcess.terminate();
    else
        kill(spotify.pid, SIGTERM);
    usleep(2e5);  // 0.2 sec
}
