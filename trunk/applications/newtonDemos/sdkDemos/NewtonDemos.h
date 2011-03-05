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

#ifndef __NEWTON_DEMOS_H__


enum ControlCommand
{
	_moveForward,
	_moveBackward,
	_moveRight,
	_moveLeft,
	_mousePick,
	_mouseMove,
	_mouseRelease,
};


class DemoEntityManager;


class newtonDemosEventFilter: public QObject 
{   
//	Q_OBJECT 
	protected:   
	bool eventFilter(QObject *obj, QEvent *ev);   
};

class newtonDemos: public QMainWindow
{
	Q_OBJECT


public:
	newtonDemos(QWidget *parent = 0, Qt::WFlags flags = 0);
	~newtonDemos();

	void LoadDemo (int index);
	static void* PhysicsAlloc (int sizeInBytes);
	static void PhysicsFree (void *ptr, int sizeInBytes);


	virtual void keyPressEvent( QKeyEvent *event);
	virtual void mouseMoveEvent ( QMouseEvent* event); 
	virtual void  mousePressEvent ( QMouseEvent* event); 
	virtual void  mouseReleaseEvent ( QMouseEvent* event);  


	public slots:
		void OnIdle(); 
		void OnLoad(); 
		void OnSave(); 
		void OnRunDemo(); 
		void OnAutoSleep();
		void OnShowDebugLines();
		void OnShowStatistics();
		void OnUseParalleSolver(); 	
		void OnShowContactPoints();
		void OnShowPhysicsProfiler();
		void OnShowThreadProfiler();
		void OnUseSimdInstructions(); 	
		void OnRunSymulationAsyncronous(); 	
		void OnSelectNumberOfMicroThreads(); 	
		void OnSetCameraSpeed();
		
		void OnNotUsed(); 

private:
	void RestoreSettings ();

	static int m_totalMemoryUsed;

	DemoEntityManager* m_canvas;
	bool m_autoSleepState;
	bool m_usesSimdInstructions;
	bool m_useParallelSolverOnSingleIslands;
	bool m_debugDisplayState;
	bool m_physicProfilerState;
	bool m_threadProfilerState;
	bool m_showContactPointState;
	bool m_showStatistics;
	bool m_doVisualUpdates;
	bool m_concurrentPhysicsUpdates;
	int m_currentThreadCount;
	int m_mousePosX;
	int m_mousePosY;
	int m_mouseSpeedX;
	int m_mouseSpeedY;

	float m_yawRate;
	float m_pitchRate;
	float m_cameraFrontSpeed;
	float m_sidewaysSpeed;

	friend class DemoEntityManager;
};



#endif