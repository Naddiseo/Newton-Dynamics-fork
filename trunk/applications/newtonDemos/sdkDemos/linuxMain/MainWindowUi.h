/********************************************************************************
** Form generated from reading UI file 'MainWindowCj2104.ui'
**
** Created: Mon Nov 22 04:14:50 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWINDOWCJ2104_H
#define MAINWINDOWCJ2104_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionShow_debug_display;
    QAction *actionHide_physics_profiler;
    QAction *actionHide_Framerate_info_bar;
    QAction *actionUse_Simd_instructions;
    QAction *actionUpdate_physics_in_mani_thread;
    QAction *actionSelect_number_of_physics_threads;
    QAction *actionExit;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuDemo_Options;
    QMenu *menuOptions;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        actionShow_debug_display = new QAction(MainWindow);
        actionShow_debug_display->setObjectName(QString::fromUtf8("actionShow_debug_display"));
        actionHide_physics_profiler = new QAction(MainWindow);
        actionHide_physics_profiler->setObjectName(QString::fromUtf8("actionHide_physics_profiler"));
        actionHide_Framerate_info_bar = new QAction(MainWindow);
        actionHide_Framerate_info_bar->setObjectName(QString::fromUtf8("actionHide_Framerate_info_bar"));
        actionUse_Simd_instructions = new QAction(MainWindow);
        actionUse_Simd_instructions->setObjectName(QString::fromUtf8("actionUse_Simd_instructions"));
        actionUpdate_physics_in_mani_thread = new QAction(MainWindow);
        actionUpdate_physics_in_mani_thread->setObjectName(QString::fromUtf8("actionUpdate_physics_in_mani_thread"));
        actionSelect_number_of_physics_threads = new QAction(MainWindow);
        actionSelect_number_of_physics_threads->setObjectName(QString::fromUtf8("actionSelect_number_of_physics_threads"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuDemo_Options = new QMenu(menubar);
        menuDemo_Options->setObjectName(QString::fromUtf8("menuDemo_Options"));
        menuOptions = new QMenu(menubar);
        menuOptions->setObjectName(QString::fromUtf8("menuOptions"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuDemo_Options->menuAction());
        menubar->addAction(menuOptions->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionExit);
        menuOptions->addAction(actionShow_debug_display);
        menuOptions->addAction(actionHide_physics_profiler);
        menuOptions->addAction(actionHide_Framerate_info_bar);
        menuOptions->addAction(actionUse_Simd_instructions);
        menuOptions->addAction(actionUpdate_physics_in_mani_thread);
        menuOptions->addAction(actionSelect_number_of_physics_threads);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Newton SDK demos", 0, QApplication::UnicodeUTF8));
        actionShow_debug_display->setText(QApplication::translate("MainWindow", "Show debug display", 0, QApplication::UnicodeUTF8));
        actionHide_physics_profiler->setText(QApplication::translate("MainWindow", "Hide physics profiler", 0, QApplication::UnicodeUTF8));
        actionHide_Framerate_info_bar->setText(QApplication::translate("MainWindow", "Hide Framerate info bar", 0, QApplication::UnicodeUTF8));
        actionUse_Simd_instructions->setText(QApplication::translate("MainWindow", "Use Simd instructions", 0, QApplication::UnicodeUTF8));
        actionUpdate_physics_in_mani_thread->setText(QApplication::translate("MainWindow", "Update physics in main thread", 0, QApplication::UnicodeUTF8));
        actionSelect_number_of_physics_threads->setText(QApplication::translate("MainWindow", "Select number of physics threads", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuDemo_Options->setTitle(QApplication::translate("MainWindow", "Demos ", 0, QApplication::UnicodeUTF8));
        menuOptions->setTitle(QApplication::translate("MainWindow", "Options", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWCJ2104_H
