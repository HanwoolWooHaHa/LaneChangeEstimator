/********************************************************************************
** Form generated from reading UI file 'lanechangeestimator.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LANECHANGEESTIMATOR_H
#define UI_LANECHANGEESTIMATOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
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

class Ui_LaneChangeEstimatorClass
{
public:
    QAction *action_Open;
    QAction *actionE_xit;
    QWidget *centralWidget;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QLabel *message;
    QFrame *line;
    QLabel *label;
    QLabel *label_2;
    QComboBox *comboBox2;
    QComboBox *comboBox3;
    QPushButton *pushButton2;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *LaneChangeEstimatorClass)
    {
        if (LaneChangeEstimatorClass->objectName().isEmpty())
            LaneChangeEstimatorClass->setObjectName(QStringLiteral("LaneChangeEstimatorClass"));
        LaneChangeEstimatorClass->resize(500, 300);
        action_Open = new QAction(LaneChangeEstimatorClass);
        action_Open->setObjectName(QStringLiteral("action_Open"));
        actionE_xit = new QAction(LaneChangeEstimatorClass);
        actionE_xit->setObjectName(QStringLiteral("actionE_xit"));
        centralWidget = new QWidget(LaneChangeEstimatorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        comboBox = new QComboBox(centralWidget);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(10, 10, 101, 31));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(130, 10, 51, 31));
        message = new QLabel(centralWidget);
        message->setObjectName(QStringLiteral("message"));
        message->setGeometry(QRect(220, 10, 270, 31));
        QFont font;
        font.setFamily(QStringLiteral("Times New Roman"));
        font.setBold(true);
        font.setWeight(75);
        message->setFont(font);
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(5, 60, 491, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 80, 41, 21));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(110, 80, 51, 21));
        comboBox2 = new QComboBox(centralWidget);
        comboBox2->setObjectName(QStringLiteral("comboBox2"));
        comboBox2->setGeometry(QRect(10, 100, 81, 31));
        comboBox3 = new QComboBox(centralWidget);
        comboBox3->setObjectName(QStringLiteral("comboBox3"));
        comboBox3->setGeometry(QRect(110, 100, 61, 31));
        pushButton2 = new QPushButton(centralWidget);
        pushButton2->setObjectName(QStringLiteral("pushButton2"));
        pushButton2->setGeometry(QRect(190, 100, 51, 31));
        LaneChangeEstimatorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(LaneChangeEstimatorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 500, 24));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        LaneChangeEstimatorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(LaneChangeEstimatorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        LaneChangeEstimatorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(LaneChangeEstimatorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        LaneChangeEstimatorClass->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menu_File->addAction(action_Open);
        menu_File->addAction(actionE_xit);

        retranslateUi(LaneChangeEstimatorClass);
        QObject::connect(pushButton2, SIGNAL(clicked()), LaneChangeEstimatorClass, SLOT(handleTypeSelected()));
        QObject::connect(pushButton, SIGNAL(clicked()), LaneChangeEstimatorClass, SLOT(handleModeSelected()));
        QObject::connect(comboBox3, SIGNAL(currentIndexChanged(int)), LaneChangeEstimatorClass, SLOT(handleMethodChanged(int)));
        QObject::connect(comboBox2, SIGNAL(currentIndexChanged(int)), LaneChangeEstimatorClass, SLOT(handleTypeChanged(int)));
        QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), LaneChangeEstimatorClass, SLOT(handleModeChanged(int)));
        QObject::connect(action_Open, SIGNAL(triggered()), LaneChangeEstimatorClass, SLOT(openMenuClicked()));

        QMetaObject::connectSlotsByName(LaneChangeEstimatorClass);
    } // setupUi

    void retranslateUi(QMainWindow *LaneChangeEstimatorClass)
    {
        LaneChangeEstimatorClass->setWindowTitle(QApplication::translate("LaneChangeEstimatorClass", "LaneChangeEstimator", 0));
        action_Open->setText(QApplication::translate("LaneChangeEstimatorClass", "&Open", 0));
        actionE_xit->setText(QApplication::translate("LaneChangeEstimatorClass", "E&xit", 0));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("LaneChangeEstimatorClass", "Mode", 0)
         << QApplication::translate("LaneChangeEstimatorClass", "Line Extractor", 0)
         << QApplication::translate("LaneChangeEstimatorClass", "Trainer", 0)
         << QApplication::translate("LaneChangeEstimatorClass", "Tester", 0)
        );
        comboBox->setCurrentText(QApplication::translate("LaneChangeEstimatorClass", "Mode", 0));
        pushButton->setText(QApplication::translate("LaneChangeEstimatorClass", "Enter", 0));
        message->setText(QApplication::translate("LaneChangeEstimatorClass", "Simulator by Hanwool WOO", 0));
        label->setText(QApplication::translate("LaneChangeEstimatorClass", "Type", 0));
        label_2->setText(QApplication::translate("LaneChangeEstimatorClass", "METHOD", 0));
        comboBox2->clear();
        comboBox2->insertItems(0, QStringList()
         << QApplication::translate("LaneChangeEstimatorClass", "Changing", 0)
         << QApplication::translate("LaneChangeEstimatorClass", "Keeping", 0)
        );
        comboBox3->clear();
        comboBox3->insertItems(0, QStringList()
         << QApplication::translate("LaneChangeEstimatorClass", "HMM", 0)
         << QApplication::translate("LaneChangeEstimatorClass", "SVM", 0)
        );
        pushButton2->setText(QApplication::translate("LaneChangeEstimatorClass", "Enter", 0));
        menu_File->setTitle(QApplication::translate("LaneChangeEstimatorClass", "&File", 0));
    } // retranslateUi

};

namespace Ui {
    class LaneChangeEstimatorClass: public Ui_LaneChangeEstimatorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LANECHANGEESTIMATOR_H
