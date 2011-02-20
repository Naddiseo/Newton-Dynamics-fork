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

	public slots:
		void OnIdle(); 
		void OnLoad(); 
		void OnSave(); 
		void OnRunDemo(); 
		void OnAutoSleep(); 	
		void OnShowDebugLines();
		void OnShowStatistics();
		void OnUseSimdInstructions(); 	
		void OnUseParalleSolver(); 	
		void OnRunSymulationAsyncronous(); 	


		
		void OnNotUsed(); 

private:
	static int m_totalMemoryUsed;

	DemoEntityManager* m_canvas;
	int m_threadCount;
	bool m_autoSleepState;
	bool m_usesSimdInstructions;
	bool m_solveIslandOnSingleThread;
	bool m_debugDisplayState;
	bool m_physicProfilerState;
	bool m_threadProfilerState;
	bool m_showStatistics;
	bool m_doVisualUpdates;
	bool m_concurrentPhysicsUpdates;

	friend class DemoEntityManager;
};







#endif