#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRect>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QCloseEvent>

#include "aboutdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

private slots:
    void toggleMovement();
    void moveMouse();
    void refreshInterval(int newInterval);
    void forceClose();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void saveSettings();

private:

    void loadSettings();


    Ui::MainWindow *ui;
    QTimer timer;
    QRect m_area;
    int m_step;

    int m_prevX, m_prevY;

    AboutDialog aboutDlg;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *actionMovement;

    QIcon iconRunning;
    QIcon iconStopped;
    QIcon icon;
};

#endif // MAINWINDOW_H
