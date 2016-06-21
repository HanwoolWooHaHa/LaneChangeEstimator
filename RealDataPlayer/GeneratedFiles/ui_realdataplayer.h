/********************************************************************************
** Form generated from reading UI file 'realdataplayer.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REALDATAPLAYER_H
#define UI_REALDATAPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RealDataPlayerClass
{
public:
    QAction *action_Load;
    QWidget *centralWidget;
    QLabel *lblFileName;
    QWidget *widget;
    QPushButton *btnReset;
    QPushButton *btnStart;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RealDataPlayerClass)
    {
        if (RealDataPlayerClass->objectName().isEmpty())
            RealDataPlayerClass->setObjectName(QStringLiteral("RealDataPlayerClass"));
        RealDataPlayerClass->resize(880, 570);
        action_Load = new QAction(RealDataPlayerClass);
        action_Load->setObjectName(QStringLiteral("action_Load"));
        centralWidget = new QWidget(RealDataPlayerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        lblFileName = new QLabel(centralWidget);
        lblFileName->setObjectName(QStringLiteral("lblFileName"));
        lblFileName->setGeometry(QRect(20, 10, 511, 21));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(20, 50, 841, 371));
        btnReset = new QPushButton(centralWidget);
        btnReset->setObjectName(QStringLiteral("btnReset"));
        btnReset->setEnabled(false);
        btnReset->setGeometry(QRect(20, 440, 41, 41));
        btnStart = new QPushButton(centralWidget);
        btnStart->setObjectName(QStringLiteral("btnStart"));
        btnStart->setEnabled(false);
        btnStart->setGeometry(QRect(780, 440, 81, 41));
        RealDataPlayerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(RealDataPlayerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 880, 24));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        RealDataPlayerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RealDataPlayerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        RealDataPlayerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(RealDataPlayerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        RealDataPlayerClass->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menu_File->addAction(action_Load);

        retranslateUi(RealDataPlayerClass);
        QObject::connect(action_Load, SIGNAL(triggered()), RealDataPlayerClass, SLOT(LoadFile()));
        QObject::connect(btnReset, SIGNAL(clicked()), RealDataPlayerClass, SLOT(ResetButtonEntered()));
        QObject::connect(btnStart, SIGNAL(clicked()), RealDataPlayerClass, SLOT(StartButtonEntered()));

        QMetaObject::connectSlotsByName(RealDataPlayerClass);
    } // setupUi

    void retranslateUi(QMainWindow *RealDataPlayerClass)
    {
        RealDataPlayerClass->setWindowTitle(QApplication::translate("RealDataPlayerClass", "RealDataPlayer", 0));
        action_Load->setText(QApplication::translate("RealDataPlayerClass", "&Load", 0));
        lblFileName->setText(QString());
        btnReset->setText(QApplication::translate("RealDataPlayerClass", "Re", 0));
        btnStart->setText(QApplication::translate("RealDataPlayerClass", "Start", 0));
        menu_File->setTitle(QApplication::translate("RealDataPlayerClass", "&File", 0));
    } // retranslateUi

};

namespace Ui {
    class RealDataPlayerClass: public Ui_RealDataPlayerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REALDATAPLAYER_H
