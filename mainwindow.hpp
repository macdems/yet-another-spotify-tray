#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtCore>
#include <QtWidgets>
#include <QtDBus/QtDBus>

class MainWindow : public QMainWindow {
    Q_OBJECT

  private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void next();
    void prev();
    void play();
    void pause();

  private:
    QAction* quitAction;

    QAction* nextAction;
    QAction* prevAction;
    QAction* playAction;
    QAction* pauseAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;

    QDBusInterface dbus;

  protected:
    void closeEvent(QCloseEvent *event) override;

  public:
    MainWindow();
};

#endif
