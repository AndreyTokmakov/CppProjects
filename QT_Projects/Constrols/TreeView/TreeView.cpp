/**============================================================================
Name        : TreeView.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TreeView
============================================================================**/

#include "TreeView.h"
#include <QFrame>
#include <QMainWindow>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QTreeView>
#include <QVBoxLayout>
#include <QApplication>

namespace TreeView {
    class Window : public QMainWindow {
    public:
        Window() {
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

            setCentralWidget(&frame);
            setWindowTitle("Tree view example");
            resize(300, 300);
        }

    private:
        QFrame frame;
        QVBoxLayout layout{&frame};
        QTreeView treeView1;
        QStandardItemModel model;
    };

    void test(int argc, char **argv)
    {
        QApplication application(argc, argv);
        Window window;
        window.show();
        QApplication::exec();
    }
}

void TreeView::TestAll(int argc, char **argv)
{
    test(argc, argv);
}