/**============================================================================
Name        : TabWidget.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TabWidget
============================================================================**/

#include "TabWidget.h"

#include <QApplication>
#include <QFrame>
#include <QMainWindow>
#include <QTabWidget>

namespace TabWidget
{
    class Window : public QMainWindow {
    public:
        Window() {
            tabWidget1.move(10, 10);
            tabWidget1.resize(370, 250);
            tabWidget1.addTab(&tabPage1, "Tab page 1");
            tabWidget1.addTab(&tabPage2, "Tab page 2");
            tabWidget1.addTab(&tabPage3, "Tab page 3");

            setCentralWidget(&frame);
            setWindowTitle("Tab widget example");
            resize(390, 270);
        }

    private:
        QFrame frame;
        QTabWidget tabWidget1 {&frame};
        QFrame tabPage1;
        QFrame tabPage2;
        QFrame tabPage3;
    };

    void test(int argc, char **argv)
    {
        QApplication application(argc, argv);
        Window window;
        window.show();
        QApplication::exec();
    }
};


void TabWidget::TestAll([[maybe_unused]] int argc,
                        [[maybe_unused]] char** argv)
{
    test(argc, argv);
};


