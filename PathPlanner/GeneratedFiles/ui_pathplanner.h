/********************************************************************************
** Form generated from reading UI file 'pathplanner.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PATHPLANNER_H
#define UI_PATHPLANNER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PathPlannerClass
{
public:
    QAction *actionGenerate;
    QWidget *centralWidget;
    QPushButton *btnStart;
    QWidget *widget;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PathPlannerClass)
    {
        if (PathPlannerClass->objectName().isEmpty())
            PathPlannerClass->setObjectName(QStringLiteral("PathPlannerClass"));
        PathPlannerClass->resize(880, 570);
        actionGenerate = new QAction(PathPlannerClass);
        actionGenerate->setObjectName(QStringLiteral("actionGenerate"));
        centralWidget = new QWidget(PathPlannerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnStart = new QPushButton(centralWidget);
        btnStart->setObjectName(QStringLiteral("btnStart"));
        btnStart->setGeometry(QRect(750, 440, 111, 51));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(20, 50, 841, 371));
        PathPlannerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PathPlannerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 880, 24));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        PathPlannerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(PathPlannerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        PathPlannerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(PathPlannerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        PathPlannerClass->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menu_File->addAction(actionGenerate);

        retranslateUi(PathPlannerClass);
        QObject::connect(btnStart, SIGNAL(clicked()), PathPlannerClass, SLOT(StartButtonEntered()));
        QObject::connect(actionGenerate, SIGNAL(triggered()), PathPlannerClass, SLOT(Generate()));

        QMetaObject::connectSlotsByName(PathPlannerClass);
    } // setupUi

    void retranslateUi(QMainWindow *PathPlannerClass)
    {
        PathPlannerClass->setWindowTitle(QApplication::translate("PathPlannerClass", "PathPlanner", 0));
        actionGenerate->setText(QApplication::translate("PathPlannerClass", "&Generate", 0));
        btnStart->setText(QApplication::translate("PathPlannerClass", "Start", 0));
        menu_File->setTitle(QApplication::translate("PathPlannerClass", "&File", 0));
    } // retranslateUi

};

namespace Ui {
    class PathPlannerClass: public Ui_PathPlannerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PATHPLANNER_H
