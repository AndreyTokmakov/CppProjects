/**============================================================================
Name        : TreeWidget.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TreeWidget
============================================================================**/

#include "TreeWidget.h"

#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace TreeWidget::DemoOne
{
    class Window : public QMainWindow
    {
    public:
        Window() {
            layout.addWidget(&treeWidget1);

            auto rootNode = new QTreeWidgetItem({"Root"});
            treeWidget1.addTopLevelItem(rootNode);
            auto firstNode = new QTreeWidgetItem({"First"});
            rootNode->addChild(firstNode);
            firstNode->addChild(new QTreeWidgetItem({"Second"}));
            firstNode->addChild(new QTreeWidgetItem({"Third"}));
            auto fourthNode = new QTreeWidgetItem({"Fourth"});
            rootNode->addChild(fourthNode);
            fourthNode->addChild(new QTreeWidgetItem({"Fifth"}));
            fourthNode->addChild(new QTreeWidgetItem({"Sixth"}));
            fourthNode->addChild(new QTreeWidgetItem({"Seventh"}));

            treeWidget1.setHeaderHidden(true);
            treeWidget1.addTopLevelItem(rootNode);
            treeWidget1.expandAll();

            setCentralWidget(&frame);
            setWindowTitle("Tree widget example");
            resize(300, 300);
        }

    private:
        QFrame frame;
        QVBoxLayout layout {&frame};
        QTreeWidget treeWidget1;
    };

    void test(int argc, char **argv)
    {
        QApplication application(argc, argv);
        Window window;
        window.show();
        QApplication::exec();
    }
}

namespace TreeWidget::DemoTwo
{

    void test(int argc, char **argv)
    {
        QApplication application(argc, argv);
        // Window window;
        // window.show();
        QApplication::exec();
    }
}
void TreeWidget::TestAll(int argc, char **argv)
{
    // DemoOne::test(argc, argv);
    DemoTwo::test(argc, argv);
}