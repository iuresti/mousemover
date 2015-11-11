#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QCursor>
#include <QSettings>

#include "defs.h"

//Windows.h is added to use mouse_event function and generate an event at a lower level than the generated for QT method QCursor::setPos
#include "windows.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent,  Qt::WindowCloseButtonHint
                | Qt::WindowMinimizeButtonHint),
    ui(new Ui::MainWindow),
    iconRunning(":/icon-running"),
    iconStopped(":/icon-stopped"),
    icon(":/icon")
{
    ui->setupUi(this);

    this->setWindowIcon(icon);

    m_step = 0;
    m_prevX = 0;
    m_prevY = 0;

    trayIcon = new QSystemTrayIcon(this);
    trayIconMenu = new QMenu(this);

    actionMovement = trayIconMenu->addAction(tr("Start"), this, SLOT(toggleMovement()));
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(tr("Show"), this, SLOT(showNormal()));
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(tr("Exit"), this, SLOT(forceClose()));

    trayIcon->setContextMenu(trayIconMenu);

    trayIcon->setIcon(iconStopped);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    connect(&timer, SIGNAL(timeout()), this, SLOT(moveMouse()));

    connect(ui->actionAbout, SIGNAL(triggered(bool)), &aboutDlg, SLOT(showNormal()));

    loadSettings();

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::loadSettings This method read the previous configuration saved using QSettings and load it to the UI
 *  also if the application was active when closed, then the mousemover is activated
 *
 */
void MainWindow::loadSettings()
{
    QSettings settings(ORG_NAME, APP_NAME);

    int offset = settings.value(OFFSET_PARAM).isValid() ? settings.value(OFFSET_PARAM).toInt() : DEFAULT_OFFSET;
    bool active = settings.value(STATUS_PARAM).isValid() ? settings.value(STATUS_PARAM).toBool() : DEFAULT_STATUS;
    int interval = settings.value(INTERVAL_PARAM).isValid() ? settings.value(INTERVAL_PARAM).toInt() : DEFAULT_INTERVAL;


   ui->intervalEdit->setValue(interval);
   ui->offset->setValue(offset);

   if((active && !timer.isActive()) || (!active && timer.isActive()))
   {
       toggleMovement();
   }
}

/**
 * @brief MainWindow::saveSettings save the current configuration using QSettings (in Windows is the registry)
 */
void MainWindow::saveSettings()
{
    QSettings settings(ORG_NAME, APP_NAME);

    settings.setValue(OFFSET_PARAM, ui->offset->value());
    settings.setValue(INTERVAL_PARAM, ui->intervalEdit->value());
    settings.setValue(STATUS_PARAM, timer.isActive());
}


/**
 * @brief MainWindow::toggleMovement Activate mousemover if it is inactive and viceversa
 */
void MainWindow::toggleMovement()
{
    QString stopText = tr("Stop");
    QString startText = tr("Start");

    if(timer.isActive())
    {
        timer.stop();
        ui->statusLabel->setText(tr("Stopped"));
        ui->startBtn->setText(startText);
        actionMovement->setText(startText);
        trayIcon->setIcon(iconStopped);
    }
    else
    {

        timer.start(ui->intervalEdit->value() * 1000);
        ui->statusLabel->setText(tr("Active"));
        ui->startBtn->setText(stopText);
        actionMovement->setText(stopText);
        trayIcon->setIcon(iconRunning);
    }

    saveSettings();
}

/**
 * @brief MainWindow::moveMouse Performs the calculations and moves the mouse
 */
void MainWindow::moveMouse()
{
    QDesktopWidget *desktop = QApplication::desktop();

    //This condition is to prevent the movement of the pointer when the user is using the mouse
    if( m_prevX == desktop->cursor().pos().x() && m_prevY == desktop->cursor().pos().y())
    {
        int offset = ui->offset->value();

        int incrementX = m_step == 0 ? offset : (m_step == 2 ? -offset : 0);
        int incrementY = m_step == 1 ? offset : (m_step == 3 ? -offset : 0);

        //This method moves the pointer but not avoid the problem of suspend for inactivity
        //desktop->cursor().setPos(desktop->cursor().pos().x() + incrementX, desktop->cursor().pos().y() + incrementY);


        //This call to the Api Win32 solves the problem of Windows getting suspended
        mouse_event(MOUSEEVENTF_MOVE, incrementX, incrementY, 0, 0);


        m_step++;
        if(m_step >= 4)
        {
            m_step = 0;
        }
    }

    m_prevX = desktop->cursor().pos().x();
    m_prevY = desktop->cursor().pos().y();

}

/**
 * @brief MainWindow::refreshInterval If the user changes in the user interface the interval then, the timer is adjusted
 * @param newInterval value in seconds to move the mouse
 */
void MainWindow::refreshInterval(int newInterval)
{
    timer.setInterval(newInterval * 1000);
}

/**
 * @brief MainWindow::closeEvent This method prevents the close action to hide the application but keep it running with the icon in the system tray
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(this->isVisible())
    {
        hide();
        event->ignore();
    }
}

/**
 * @brief MainWindow::forceClose This method is called for the "exit" option in the context menu of the icon in system tray to close the application
 */
void MainWindow::forceClose()
{
    hide();
    close();
}

/**
 * @brief MainWindow::iconActivated Method to handle events to the icon in system tray
 * @param reason
 */
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
        showNormal();
        break;
    default:
        break;
    }
}
