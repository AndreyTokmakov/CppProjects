/**============================================================================
Name        : Experiments.cpp
Created on  : 2/14/23
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : QT Experiments
============================================================================**/

#include "Experiments.h"

#include <iostream>

#include <QtCore>
#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QLCDNumber>
#include <QSlider>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>

#include <QAction>
#include <QDebug>
#include <QFrame>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>
#include <QStyle>

namespace Experiments
{
    void MinimalWindow(int argc,  char** argv)
    {
        QApplication app(argc, argv);
        QWidget window;

        window.resize(250, 150);
        window.setWindowTitle("Simple example");
        window.show();

        QApplication::exec();
    }

    void MinimalWindow_WithToolTip(int argc,  char** argv)
    {
        QApplication app(argc, argv);
        QWidget window;

        window.resize(250, 150);
        window.move(300, 300);
        window.setWindowTitle("ToolTip"); // устанавливаем заголовок для окна
        window.setToolTip("QWidget"); // устанавливаем всплывающую подсказку для виджета
        window.show(); // выводим на экран

        QApplication::exec();
    }

    void MinimalWindow_Icon(int argc,  char** argv)
    {
        QApplication app(argc, argv);
        QWidget window;

        window.resize(250, 150);
        window.setWindowTitle("Icon");
        window.setWindowIcon(QIcon(R"(/home/andtokm/DiskS/Icons/icons8-process-64.png)"));
        window.show();

        QApplication::exec();
    }
    int simpleButton(int argc, char *argv[])
    {
        QApplication app(argc, argv);
        QPushButton hello("Hello world!");
        hello.resize(100, 30);
        hello.show();

        return QApplication::exec();
    }

    int quitButton(int argc, char** argv)
    {
        QApplication app(argc, argv);

        QPushButton btnQuit("Quit");
        btnQuit.resize(175, 30);
        btnQuit.setFont(QFont("Times", 18, QFont::Bold));

        QObject::connect(&btnQuit, SIGNAL(clicked()), &app, SLOT(quit()));

        btnQuit.show();

        return QApplication::exec();
    }

    void windowWithButton(int argc,  char** argv)
    {
        QApplication app(argc, argv);
        QWidget window;

        QPushButton btnQuit("Quit", &window);
        btnQuit.setFont(QFont("Times", 18, QFont::Bold));
        btnQuit.setGeometry(150, 80, 100, 40);
        QObject::connect(&btnQuit, SIGNAL(clicked()), &app, SLOT(quit()));

        window.resize(400, 200);
        window.show();

        QApplication::exec();
    }


    class WidgetWithSlider : public QWidget
    {
    public:
        WidgetWithSlider(QWidget *parent = nullptr): QWidget(parent)
        {
            QPushButton *quit = new QPushButton(tr("Quit"));
            quit->setFont(QFont("Times", 18, QFont::Bold));

            QLCDNumber *lcd = new QLCDNumber(2);
            lcd->setSegmentStyle(QLCDNumber::Filled);

            QSlider *slider = new QSlider(Qt::Horizontal);
            slider->setRange(0, 99);
            slider->setValue(0);

            connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));
            connect(slider, SIGNAL(valueChanged(int)),lcd, SLOT(display(int)));

            QVBoxLayout *layout = new QVBoxLayout;
            layout->addWidget(quit);
            layout->addWidget(lcd);
            layout->addWidget(slider);

            setLayout(layout);
        }
    };

    void widgetWithSlider(int argc, char *argv[])
    {
        QApplication app(argc, argv);
        WidgetWithSlider widget;
        widget.show();
        QApplication::exec();
    }
};

namespace MessageBoxTests
{
    class Window1 : public QMainWindow {
    //Q_OBJECT
    public:
        Window1() {
            buttonClickMe.setText("Click me");
            buttonClickMe.move(10, 10);
            connect(&buttonClickMe, &QPushButton::clicked, [&] {
                QMessageBox(QMessageBox::Icon::NoIcon, "", "Hello, World!").exec();
            });

            setCentralWidget(&frame);
            setWindowTitle("Hello world (Message box)");
            resize(300, 300);
        }

    private:
        QFrame frame;
        QPushButton buttonClickMe {&frame};
    };

    void SimpleMessageBox(int argc, char *argv[])
    {
        QApplication application(argc, argv);
        Window1 window1;
        window1.show();
        QApplication::exec();
    }
}


namespace StatusBarTests
{
    class Window : public QMainWindow
    {
    public:
        Window() {
            QStatusBar* statusBar = this->statusBar();
            statusBar->showMessage("Status bar...");

            setCentralWidget(&frame);
            setWindowTitle("Status bar example");
            resize(300, 300);
        }

    private:
        QFrame frame;
    };

    void TestStatusBar(int argc, char *argv[])
    {
        QApplication application(argc, argv);
        Window window1;
        window1.show();
        QApplication::exec();
    }
}


namespace MenusBarTests
{
    class Window : public QMainWindow {
    public:
        Window() {
            QMenu* menuFile = menuBar()->addMenu("&File");
            menuFile->addAction(style()->standardIcon(QStyle::StandardPixmap::SP_FileIcon), "&New", this, &Window::OnMenuFileNewClick, QKeySequence(Qt::CTRL + Qt::Key_N));
            menuFile->addAction(style()->standardIcon(QStyle::StandardPixmap::SP_DirOpenIcon), "&Open", this, &Window::OnMenuFileOpenClick, QKeySequence(Qt::CTRL + Qt::Key_O));
            menuFile->addSeparator();
            menuFile->addAction(style()->standardIcon(QStyle::StandardPixmap::SP_DialogSaveButton), "&Save", this, &Window
            ::OnMenuFileSaveClick, QKeySequence(Qt::CTRL + Qt::Key_S));
            menuFile->addAction("Save &As...", this, &Window::OnMenuFileSaveAsClick);
            menuFile->addSeparator();
            menuFile->addAction(QIcon::fromTheme("document-print"), "&Print", this, &Window::OnMenuFileSaveClick, QKeySequence(Qt::CTRL + Qt::Key_P));
            menuFile->addAction(QIcon::fromTheme("document-print-preview"), "Print preview");
            menuFile->addSeparator();
            menuFile->addAction("&Exit", this, &Window::OnMenuFileCloseClick, QKeySequence(Qt::ALT + Qt::Key_F4));

            QMenu* menuEdit = menuBar()->addMenu("&Edit");
            menuEdit->addAction("&Undo", this, &Window::OnMenuEditUndoClick, QKeySequence(Qt::CTRL + Qt::Key_Z));
            menuEdit->addAction("&Redo", this, &Window::OnMenuEditRedoClick, QKeySequence(Qt::CTRL + Qt::Key_Y));
            menuEdit->addSeparator();
            menuEdit->addAction(QIcon::fromTheme("edit-cut"), "&Cut", this, &Window::OnMenuEditCutClick, QKeySequence(Qt::CTRL + Qt::Key_X));
            menuEdit->addAction(QIcon::fromTheme("edit-copy"), "&Copy", this, &Window::OnMenuEditCopyClick, QKeySequence(Qt::CTRL + Qt::Key_C));
            menuEdit->addAction(QIcon::fromTheme("edit-paste"), "&Paste", this, &Window::OnMenuEditPasteClick, QKeySequence(Qt::CTRL + Qt::Key_V));
            menuEdit->addSeparator();
            menuEdit->addAction("Select &All", this, &Window::OnMenuEditSelectAllClick, QKeySequence(Qt::CTRL + Qt::Key_A));

            QMenu* menuHelp = menuBar()->addMenu("&Help");
            menuHelp->addAction("&About", this, &Window::OnMenuHelpAboutClick);

            setCentralWidget(&frame);
            setWindowTitle("Main menu example");
            resize(300, 300);
        }

    private:
        void OnMenuFileNewClick() {qDebug() << "MainMenu/File/New";}
        void OnMenuFileOpenClick() {qDebug() << "MainMenu/File/Open";}
        void OnMenuFileSaveClick() {qDebug() << "MainMenu/File/Save";}
        void OnMenuFileSaveAsClick() {qDebug() << "MainMenu/File/SaveAs";}
        void OnMenuFileCloseClick() {qDebug() << "MainMenu/File/Close";}
        void OnMenuEditUndoClick() {qDebug() << "MainMenu/Edit/Undo";}
        void OnMenuEditRedoClick() {qDebug() << "MainMenu/Edit/Redo";}
        void OnMenuEditCutClick() {qDebug() << "MainMenu/Edit/Cut";}
        void OnMenuEditCopyClick() {qDebug() << "MainMenu/Edit/Copy";}
        void OnMenuEditPasteClick() {qDebug() << "MainMenu/Edit/Paste";}
        void OnMenuEditDeletaClick() {qDebug() << "MainMenu/Edit/delete";}
        void OnMenuEditSelectAllClick() {qDebug() << "MainMenu/Edit/SelectAll";}
        void OnMenuHelpAboutClick() {QMessageBox::about(this, "About", "MainMenu example.\nVersion 1.0.0\n\n@ 2020 by Gammasoft.");}

        QFrame frame;
    };

    void MenusBarTest(int argc, char *argv[])
    {
        QApplication application(argc, argv);
        Window window;
        window.show();
        QApplication::exec();
    }
}


namespace Experiments::Threads
{
    class MyThread : public QThread
    {
    private:
        void run() override
        {
            for (int i = 0; i < 10; ++i)
            {
                // std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << i << std::endl;
            }
        }
    };

    void SimpleThread()
    {
        MyThread* thread = new MyThread();
        thread->start();    // starts a new thread which calls run()
        thread->wait();     // waits for the thread to finish

        delete thread;
    }

}


void Experiments::TestAll(int argc, char **argv)
{
    // MinimalWindow(argc, argv);
    // MinimalWindow_WithToolTip(argc, argv);
    // MinimalWindow_Icon(argc, argv);

    // simpleButton(argc, argv);
    // quitButton(argc, argv);
    // windowWithButton(argc, argv);
    // widgetWithSlider(argc, argv);

    // MessageBoxTests::SimpleMessageBox(argc, argv);

    // StatusBarTests::TestStatusBar(argc, argv);

    // MenusBarTests::MenusBarTest(argc, argv);

    Threads::SimpleThread();
}