#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QtCore>
#include <QtWidgets>
#include <QtDBus/QtDBus>

class SpotifyFrame : public QMainWindow {
    Q_OBJECT

  private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void next();
    void prev();
    void play();
    void pause();

    void dbusPropertiesChanged(const QString& name, const QVariantMap& properties, const QStringList&);

  public slots:
    void activate();

  private:
    QAction* quitAction;

    QAction* nextAction;
    QAction* prevAction;
    QAction* playAction;
    QAction* pauseAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;

    QDBusInterface mpris;

  protected:
    void closeEvent(QCloseEvent *event) override;

  public:
    SpotifyFrame();
};

#endif
