#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QCursor>
#include <QSettings>

#include "defs.h"


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
    connect(&timer, SIGNAL(timeout()), this, SLOT(moveMouse()));

    connect(ui->actionAcerca_de, SIGNAL(triggered(bool)), &aboutDlg, SLOT(showNormal()));

    trayIcon = new QSystemTrayIcon(this);

    trayIconMenu = new QMenu(this);

    actionMovement = trayIconMenu->addAction("Iniciar", this, SLOT(toggleMovement()));
    trayIconMenu->addSeparator();
    trayIconMenu->addAction("Mostrar", this, SLOT(showNormal()));
    trayIconMenu->addSeparator();
    trayIconMenu->addAction("Salir", this, SLOT(forceClose()));

    trayIcon->setContextMenu(trayIconMenu);

    trayIcon->setIcon(iconStopped);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    m_prevX = 0;
    m_prevY = 0;

    loadSettings();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    QSettings settings(ORG_NAME, APP_NAME);

    int offset = settings.value(OFFSET_PARAM).isValid() ? settings.value(OFFSET_PARAM).toInt() : 50;
    bool active = settings.value(STATUS_PARAM).isValid() ? settings.value(STATUS_PARAM).toBool() : false;
    int interval = settings.value(INTERVAL_PARAM).isValid() ? settings.value(INTERVAL_PARAM).toInt() : 3;


   ui->intervalEdit->setValue(interval);
   ui->offset->setValue(offset);

   if((active && !timer.isActive()) || (!active && timer.isActive()))
   {
       toggleMovement();
   }
}

void MainWindow::saveSettings()
{
    QSettings settings(ORG_NAME, APP_NAME);

    settings.setValue(OFFSET_PARAM, ui->offset->value());
    settings.setValue(INTERVAL_PARAM, ui->intervalEdit->value());
    settings.setValue(STATUS_PARAM, timer.isActive());
}

void MainWindow::toggleMovement()
{
    QString stopText = tr("Detener");
    QString startText = tr("Iniciar");

    if(timer.isActive())
    {
        timer.stop();
        ui->statusLabel->setText(tr("Detenido"));
        ui->startBtn->setText(startText);
        actionMovement->setText(startText);
        trayIcon->setIcon(iconStopped);
    }
    else
    {

        timer.start(ui->intervalEdit->value() * 1000);
        ui->statusLabel->setText(tr("Activo"));
        ui->startBtn->setText(stopText);
        actionMovement->setText(stopText);
        trayIcon->setIcon(iconRunning);
    }

    saveSettings();
}

void MainWindow::moveMouse()
{
    QDesktopWidget *desktop = QApplication::desktop();

    if( m_prevX == desktop->cursor().pos().x() && m_prevY == desktop->cursor().pos().y())
    {
        int offset = ui->offset->value();

        int incrementX = m_step == 0 ? offset : (m_step == 2 ? -offset : 0);
        int incrementY = m_step == 1 ? offset : (m_step == 3 ? -offset : 0);

        desktop->cursor().setPos(desktop->cursor().pos().x() + incrementX, desktop->cursor().pos().y() + incrementY);

        m_step++;
        if(m_step >= 4)
        {
            m_step = 0;
        }
    }

    m_prevX = desktop->cursor().pos().x();
    m_prevY = desktop->cursor().pos().y();

}

void MainWindow::refreshInterval(int newInterval)
{
    timer.setInterval(newInterval * 1000);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(this->isVisible())
    {
        hide();
        event->ignore();
    }
}

void MainWindow::forceClose()
{
    hide();
    close();
}


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
