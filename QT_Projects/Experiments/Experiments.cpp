/**============================================================================
Name        : Experiments.cpp
Created on  : 2/14/23
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : QT Experiments
============================================================================**/

#include "Experiments.h"

#include <QtCore>
#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QLCDNumber>
#include <QSlider>
#include <QVBoxLayout>

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

void Experiments::TestAll(int argc, char **argv)
{
    // MinimalWindow(argc, argv);
    // simpleButton(argc, argv);
    // quitButton(argc, argv);
    // windowWithButton(argc, argv);
    widgetWithSlider(argc, argv);
}