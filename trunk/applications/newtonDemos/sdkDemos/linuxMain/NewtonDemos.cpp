/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

// NewtonDemos.cpp : Defines the entry point for the application.
//



#include <QtGui/QtGui>
#include <QtGui/QMainWindow>
#include "MainWindowUi.h"


class MainWindow: public QMainWindow, public Ui::MainWindow
{

//	Q_OBJECT
	public:
	MainWindow ()
		:QMainWindow ()
	{
		resize (1024, 768);
		setupUi (this);
	}	

};


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}

