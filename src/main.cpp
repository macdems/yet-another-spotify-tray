#include <unistd.h>
#include <signal.h>

#include <QtWidgets>

#include "app.hpp"

// Shared mutex to prevent running the app twice
QSharedMemory mutex("yet-another-spotify-tray");

// Main application
SpotifyTrayApp* app = nullptr;

// Try to kill Spotify process if got SIGTERM
void sigHandler(int signal) {
    switch (signal) {
        case SIGINT:
        case SIGABRT:
        case SIGQUIT:
        case SIGKILL:
        case SIGTERM:
            delete app;
            mutex.detach();
            exit(128 + signal);
        case SIGUSR1:
            if (app && app->mainFrame) app->mainFrame->activate();
    }
}

int main(int argc, char** argv) {
    unsigned long pid = 0;
    if (mutex.attach() && mutex.lock()) {
        pid = *reinterpret_cast<unsigned long*>(mutex.data());
        mutex.unlock();
    }
    mutex.detach();
    if (mutex.create(sizeof(unsigned long))) {
        pid = getpid();
        if (mutex.lock()) {
            *reinterpret_cast<unsigned long*>(mutex.data()) = pid;
            mutex.unlock();
        }
    } else {
        auto error = mutex.error();
        if (error == QSharedMemory::AlreadyExists) {
            if (pid != 0) {
                qInfo("Spotify Tray is already running (PID %lu) - showing window", pid);
                kill(pid, SIGUSR1);
                return 0;
            } else {
                qCritical("Spotify Tray is already running, but I cannot determine its PID");
                return 1;
            }
        }
        pid = getpid();
    }

    signal(SIGTERM, sigHandler);
    signal(SIGINT, sigHandler);
    signal(SIGUSR1, sigHandler);

    int exitcode;

    qDebug("Starting Spotify Tray (PID %lu)", pid);
    try {
        app = new SpotifyTrayApp(argc, argv);
        exitcode = app->exec();
    } catch (int e) {
        return e;
    }

    mutex.detach();

    delete app;
    return exitcode;
}
