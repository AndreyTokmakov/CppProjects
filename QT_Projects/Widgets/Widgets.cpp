/**============================================================================
Name        : Widgets.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : QT Widgets
============================================================================**/

#include "Widgets.h"

#include <QtCore>
#include <QWidget>
#include <QApplication>
#include <QPushButton>


namespace Widgets
{
    class MyWidget : public QWidget
    {
    public:
        explicit MyWidget(QWidget *parent = nullptr): QWidget(parent)
        {
            setFixedSize(200, 120);

            auto *quit = new QPushButton(tr("Quit"), this);
            quit->setGeometry(62, 40, 75, 30);
            quit->setFont(QFont("Times", 18, QFont::Bold));

            connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));
        }
    };


    void simpleWidgetWindow(int argc, char *argv[])
    {
        QApplication app(argc, argv);
        MyWidget widget;
        widget.show();
        QApplication::exec();
    }
}


void Widgets::TestAll(int argc, char **argv)
{
    simpleWidgetWindow(argc, argv);
}