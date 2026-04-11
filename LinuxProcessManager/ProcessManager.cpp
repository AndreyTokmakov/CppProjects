/**============================================================================
Name        : ProcessManager.cpp
Created on  : 11.09.2023
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : v
============================================================================**/


#include "ProcessManager.h"


#define DISABLE

#ifndef DISABLE

#include <optional>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <fstream>
#include <functional>
#include <filesystem>
#include <tuple>
#include <ranges>

#include <exception>
#include <thread>
#include <future>
#include <mutex>
#include <syncstream>
#include <utility>

#include <numeric>

#include <utility>
#include <vector>
#include <any>
#include <list>
#include <forward_list>
#include <deque>
#include <map>
#include <algorithm>
#include <array>
#include <version>
#include <concepts>
#include <span>
#include <cmath>
#include <stack>
#include <variant>
#include <chrono>

#include <QtCore>
#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
// #include <QDebug>
#include <QFrame>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QStyle>
#include <QStandardItem>
#include <QHeaderView>
#include <QTreeView>
#include <QLabel>
#include <QProgressBar>

/*
class ClockThread : public QThread
{
Q_OBJECT
signals:
    void sendTime(QString time);

private:
    void run() override
    {
        QTimer timer;
        connect(&timer, SIGNAL(timeout()), this, SLOT(timerHit()), Qt::DirectConnection);
        timer.setInterval(10);
        timer.start();   // puts one event in the threads event queue
        exec();
        timer.stop();
    }

    QString m_lastTime;

private slots:

    void timerHit()
    {
        QString newTime = QDateTime::currentDateTime().toString("ddd MMMM d yy, hh:mm:ss");
        if (m_lastTime != newTime ){
            m_lastTime = newTime;
            emit sendTime(newTime) ;
        }
    }
};
*/


class Application : public QApplication
{
public:
    Application(int &argc, char **argv) : QApplication(argc, argv) {
        enableDarkMode();
    }

private:
    void enableDarkMode()
    {
#ifndef Q_OS_MACOS
        qApp->setStyle("Fusion");
        QPalette darkPalette = QPalette();
        const QColor darkColor = QColor(65, 65, 65);
        const QColor baseColor = QColor(48,48,48);
        const QColor disabledColor = QColor(127,127,127);

        darkPalette.setColor(QPalette::Window, darkColor);
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, baseColor);
        darkPalette.setColor(QPalette::AlternateBase, darkColor);
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        darkPalette.setColor(QPalette::Button, darkColor);
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

        qApp->setPalette(darkPalette);
        qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
#endif
    }
};



class Window : public QMainWindow
{
public:

    Window()
    {
        /** Init TreeView:: **/
        layout.addWidget(&treeView1);

        auto rootNode = new QStandardItem("Root");
        model.appendRow(rootNode);
        auto firstNode = new QStandardItem("First");
        rootNode->appendRow(firstNode);
        firstNode->appendRow(new QStandardItem("Second"));
        firstNode->appendRow(new QStandardItem("Third"));
        auto fourthNode = new QStandardItem("Fourth");
        rootNode->appendRow(fourthNode);
        fourthNode->appendRow(new QStandardItem("Fifth"));
        fourthNode->appendRow(new QStandardItem("Sixth"));
        fourthNode->appendRow(new QStandardItem("Seventh"));

        treeView1.setHeaderHidden(true);
        treeView1.setModel(&model);
        treeView1.expandAll();

        // TODO: Refactor this
        QLabel *statusLabel = new QLabel(this);

        QProgressBar *statusProgressBar = new QProgressBar(this);

        // statusProgressBar->move(50, 110);
        // statusProgressBar->resize(100, 25);
        statusProgressBar->setMinimum(0);
        statusProgressBar->setMaximum(100);
        statusProgressBar->setValue(50);
        statusProgressBar->setTextVisible(false);
        statusProgressBar->setToolTip(QString("Blah-Blah-Blah"));

        //statusProgressBar->setStyleSheet(QString("QStatusBar::item{border: 0px}"));

        statusLabel->setText("Status Label");

        /** Add status bar: **/
        // status->showMessage("Status bar...");
        status->addPermanentWidget(statusLabel);
        status->addPermanentWidget(statusProgressBar);


        QMenu* menuFile = menu->addMenu("&File");
        menuFile->addAction(style()->standardIcon(QStyle::StandardPixmap::SP_FileIcon),"&New",this, &Window::OnMenuItemClick);
        menuFile->addAction(style()->standardIcon(QStyle::StandardPixmap::SP_DirOpenIcon), "&Open",this, &Window::OnMenuItemClick);

        menuFile->addSeparator();

        menuFile->addAction(style()->standardIcon(QStyle::StandardPixmap::SP_DialogSaveButton),"&Save", this, &Window::OnMenuItemClick);
        menuFile->addAction("Save &As...", this,&Window::OnMenuItemClick);

        menuFile->addSeparator();

        menuFile->addAction("&Exit", this,&Window::OnMenuItemClick);

        QMenu* menuEdit = menu->addMenu("&Edit");
        menuEdit->addAction("&Undo", this,&Window::OnMenuItemClick);
        menuEdit->addAction("&Redo", this,&Window::OnMenuItemClick);

        menuEdit->addSeparator();

        menuEdit->addAction(QIcon::fromTheme("edit-cut"), "&Cut",this, &Window::OnMenuItemClick);
        menuEdit->addAction(QIcon::fromTheme("edit-copy"), "&Copy",this, &Window::OnMenuItemClick);
        menuEdit->addAction(QIcon::fromTheme("edit-paste"), "&Paste",this, &Window::OnMenuItemClick);

        menuEdit->addSeparator();

        menuEdit->addAction("Select &All", this,&Window::OnMenuItemClick);

        QMenu* menuHelp = menu->addMenu("&Help");
        menuHelp->addAction("&About", this, &Window::OnMenuHelpAboutClick);


        setCentralWidget(&frame);
        setWindowTitle("Tree view example");
        //resize(300, 300);

        // thread.start();
        // thread.wait();
    }

private:
    void OnMenuItemClick() {
        status->showMessage("Status bar...wwwww");
    }

    void OnMenuHelpAboutClick() {
        QMessageBox::about(this, "About", "MainMenu example.\nVersion 1.0.0\n\n@ 2020 by Gammasoft.");
    }

private:
    QFrame frame;
    QVBoxLayout layout{&frame};
    QTreeView treeView1;
    QStandardItemModel model;

    // MyThread thread {" A"};

    const std::unique_ptr<QStatusBar> status { statusBar() };
    const std::unique_ptr<QMenuBar> menu { menuBar() };
};

void runApp(int argc, char **argv)
{
    Application application(argc, argv);
    Window window;
    window.resize(1200, 800);
    window.show();
    QApplication::exec();
}


#endif

void ProcessManager::run(int argc, char **argv)
{
    // runApp(argc, argv);

}

#ifndef DISABLE

#include "ProcessManager.moc"

#endif
