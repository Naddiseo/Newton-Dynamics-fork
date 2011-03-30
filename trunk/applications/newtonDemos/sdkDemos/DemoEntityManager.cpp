#include "toolbox_stdafx.h"
#include "NewtonDemos.h"
#include "DemoEntityManager.h"
#include "SkyBox.h"
#include "DemoMesh.h"
#include "DemoEntity.h"
#include "DemoCamera.h"
#include "PhysicsUtils.h"
#include "DebugDisplay.h"
#include "dRuntimeProfiler.h"
#include "OpenGlUtil.h"

#define MAX_PHYSICS_LOOPS 1
#define MAX_PHYSICS_FPS	  120.0f

#define PROJECTILE_INITIAL_SPEED 20.0f


DemoEntityManager::DemoEntityManager(QWidget* const parent, QGLFormat& format)
	:QGLWidget (format, parent)
	,dList <DemoEntity*>() 
	,dSimulationThread()
	,m_camera(NULL)
	,m_world(NULL)
	,m_physicsUpdate(true) 
	,m_reEntrantUpdate (false)
	,m_microsecunds (0)
	,m_cannonBallRate(0)
	,m_profiler (620 * 0 / 8 + 45, 40)
	,m_font()
	,m_navegationQueueLock(0)
	,m_navegationQueueCount(0)
	,m_timerId(0)
	,m_meshBallMesh(NULL)
{
	// Create the main Camera
	m_camera = new DemoCamera();

	// initialized the physics world for the new scene
	Cleanup ();

	// Start rendering after the system is set properly
	ContinueExecution();

	doubleBuffer(); 
	setAutoFillBackground(false);

#ifdef _MAC_VER		
	m_timerId = startTimer(0);
#endif
}

DemoEntityManager::~DemoEntityManager(void)
{
#ifdef _MAC_VER	
	killTimer(m_timerId);
#endif

	if (m_meshBallMesh) {
		m_meshBallMesh->Release();
	}

	delete m_camera;

	// suspend simulation before making changes to the physics world
	StopsExecution ();

	Cleanup ();
	if (m_world) {
		NewtonDestroy (m_world);
		m_world = NULL;
	}
	_ASSERTE (NewtonGetMemoryUsed () == 0);

	// Start rendering after the system is set properly
	ContinueExecution();
}

//	GLContext& GetGL() {return m_glContext;}
NewtonWorld* DemoEntityManager::GetNewton() const 
{
	return m_world;
}

DemoCamera* DemoEntityManager::GetCamera___() const 
{
	return m_camera;
}

void DemoEntityManager::SetCameraMatrix (const dQuaternion& rotation, const dVector& position)
{
	dMatrix matrix (rotation, position);
	m_cameraPitch = dAsin (matrix.m_front.m_y);
	m_cameraYaw = dAtan2 (-matrix.m_front.m_z, matrix.m_front.m_x);

	m_camera->SetMatrix (*this, rotation, position);
	m_camera->SetMatrix (*this, rotation, position);
}

void DemoEntityManager::initializeGL()
{
	QGLWidget::initializeGL();

	m_camera->SetProjectionMode(width(), height());
}


void DemoEntityManager::resizeGL(int w, int h)
{
	QGLWidget::resizeGL(w, h);
	m_camera->SetProjectionMode(width(), height());
}

void DemoEntityManager::Print (QPainter& painter, int x, int y, const char *fmt, ... ) const
{
	va_list argptr;
	char string[2048];

	va_start (argptr, fmt);
	vsprintf (string, fmt, argptr);
	va_end( argptr );
	
	painter.drawText(x, y, tr(string));
}


void DemoEntityManager::RemoveEntity (dListNode* const entNode)
{
	DemoEntity* const entity = entNode->GetInfo();
	entity->Release();
	Remove(entNode);
}

void DemoEntityManager::RemoveEntity (DemoEntity* const ent)
{
	for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
		if (node->GetInfo() == ent) {
			RemoveEntity (node);
			break;
		}
	}
}

void DemoEntityManager::Cleanup ()
{
	// suspend simulation before making changes to the physics world
	StopsExecution ();

	// destroy all remaining visual objects
	//	demo.CleanUp();
	while (GetFirst()) {
		RemoveEntity (GetFirst());
	}

	// destroy the Newton world
	if (m_world) {
		NewtonDestroy (m_world);
		m_world = NULL;
	}

	//	memset (&demo, 0, sizeof (demo));
	// check that there are no memory leak on exit
	_ASSERTE (NewtonGetMemoryUsed () == 0);

//	newtonDemos* const mainWindow = (newtonDemos*) parent();

	// create the newton world
	m_world = NewtonCreate();

	// set the default parameters for the newton world
	// set the simplified solver mode (faster but less accurate)
	NewtonSetSolverModel (m_world, 1);

/*
	// Set the number of internal micro threads
	NewtonSetThreadsCount (m_world, mainWindow->m_threadCount);

	// use the standard x87 floating point model  
	if (mainWindow->m_usesSimdInstructions) {
		NewtonSetPlatformArchitecture (m_world, 3);  //best hardware (SSE at this time)
	} else {
		NewtonSetPlatformArchitecture (m_world, 0);  //x87 mode
	}

	// set the parallel solve on single island or or off
	NewtonSetMultiThreadSolverOnSingleIsland (m_world, mainWindow->m_useParallelSolverOnSingleIslands ? 1 : 0);
*/
//	mainWindow->RestoreSettings();

	// set a fix world size
	dVector minSize (-500.0f, -500.0f, -500.0f);
	dVector maxSize ( 500.0f,  500.0f,  500.0f);
	NewtonSetWorldSize (m_world, &minSize[0], &maxSize[0]); 

	// set the performance track function
	NewtonSetPerformanceClock (m_world, dRuntimeProfiler::GetTimeInMicrosenconds);

	// Set performance counters off
	memset (m_showProfiler, 1, sizeof (m_showProfiler));

	// clean up all caches the engine have saved
	NewtonInvalidateCache (m_world);

	// Set the Newton world user data
	NewtonWorldSetUserData(m_world, this);

	// Start rendering after the system is set properly
	ContinueExecution();
}



void DemoEntityManager::LoadScene (const char* const fileName)
{
	dScene database (GetNewton());
	
	// stops simulation
	StopsExecution (); 

	database.Deserialize(fileName);

	// Load the Visual Scene
	EntityDictionary entDictionary;
	LoadVisualScene(&database, entDictionary);

	//Load the physics world
	dList<NewtonBody*> bodyList;
	database.SceneToNewtonWorld(m_world, bodyList);

	// bind every rigidBody loaded to the scene entity
	for (dList<NewtonBody*>::dListNode* bodyNode = bodyList.GetFirst(); bodyNode; bodyNode = bodyNode->GetNext()) {
		// find the user data and set to the visual entity in the scene
		NewtonBody* const body = bodyNode->GetInfo();
		dScene::dTreeNode* const sceneNode = (dScene::dTreeNode*)NewtonBodyGetUserData(body);
		DemoEntity* const entity = entDictionary.Find(sceneNode)->GetInfo();
		NewtonBodySetUserData(body, entity);

		// see if this body have some special setups
		dScene::dTreeNode* const node = database.FindChildByType(sceneNode, dRigidbodyNodeInfo::GetRttiType());
		_ASSERTE (node);
		dRigidbodyNodeInfo* const bodyData = (dRigidbodyNodeInfo*) database.GetInfoFromNode(node);
		dVariable* bodyType = bodyData->FindVariable("rigidBodyType");

		// set the default call backs
		if (!bodyType || !strcmp (bodyType->GetString(), "default gravity")) {
			NewtonBodySetTransformCallback(body, DemoEntity::SetTransformCallback);
			NewtonBodySetForceAndTorqueCallback(body, PhysicsApplyGravityForce);
			NewtonBodySetDestructorCallback (body, PhysicsBodyDestructor);
		}
	}


	// clean up all caches the engine have saved
	NewtonInvalidateCache (m_world);

	// resume the simulation
	ContinueExecution();
}

void DemoEntityManager::SaveScene (const char* const name)
{
	_ASSERTE (0);
/*
	// stops simulation
	StopsExecution (); 

	dScene database (GetNewton());
	database.NewtonWorldToScene (GetNewton());

	char fileName[2048];
	GetWorkingFileName (name, fileName);
	database.Serialize(fileName);

	// resume the simulation
	ContinueExecution();
*/
}


void DemoEntityManager::LoadVisualScene(dScene* const scene, EntityDictionary& dictionary)
{
	// load all meshes into a Mesh cache for reuse
	dTree<DemoMesh*, dScene::dTreeNode*> meshDictionary;
	for (dScene::dTreeNode* node = scene->GetFirstNode (); node; node = scene->GetNextNode (node)) {
		dNodeInfo* info = scene->GetInfoFromNode(node);
		if (info->GetTypeId() == dMeshNodeInfo::GetRttiType()) {
			DemoMesh* const mesh = new DemoMesh(scene, node);
			meshDictionary.Insert(mesh, node);
		}
	}


	// create an entity for every root node in the mesh
	dScene::dTreeNode* const root = scene->GetRootNode();
	for (void* child = scene->GetFirstChild(root); child; child = scene->GetNextChild (root, child)) {
		dScene::dTreeNode* node = scene->GetNodeFromLink(child);
		dNodeInfo* info = scene->GetInfoFromNode(node);
		if (info->GetTypeId() == dSceneNodeInfo::GetRttiType()) {
			// Load this model and add it to the Scene
			DemoEntity* const entityRoot = new DemoEntity (*this, scene, node, meshDictionary, dictionary);
			Append(entityRoot);
		}
	}

	// release all meshes before exiting
	dTree<DemoMesh*, dScene::dTreeNode*>::Iterator iter (meshDictionary);
	for (iter.Begin(); iter; iter++) {
		DemoMesh* const mesh = iter.GetNode()->GetInfo();
		mesh->Release();
	}
}


void DemoEntityManager::SetAutoSleepState (bool state)
{
	int autosleep = state ? 1 : 0;
	for (NewtonBody* body = NewtonWorldGetFirstBody(m_world); body; body = NewtonWorldGetNextBody(m_world, body)) {
		NewtonBodySetAutoSleep(body, autosleep);
	}
}


void DemoEntityManager::InterpolateMatrices ()
{
	// calculate the fraction of the time step for intepolations
	unsigned64 timeStep = dGetTimeInMicrosenconds () - m_microsecunds;		
	dFloat step = (dFloat (timeStep) * MAX_PHYSICS_FPS) / 1.0e6f;
	_ASSERTE (step >= 0.0f);
	if (step > 1.0f) {
		step = 1.0f;
	}
	
	// interpolate the Camera matrix;
	m_camera->InterpolateMatrix (*this, step);

	// interpolate the location of all entities in the world
	for (NewtonBody* body = NewtonWorldGetFirstBody(m_world); body; body = NewtonWorldGetNextBody(m_world, body)) {
		DemoEntity* entity = (DemoEntity*)NewtonBodyGetUserData(body);
		entity->InterpolateMatrix (*this, step);
	}
}

void DemoEntityManager::ResetTimer()
{
	dResetTimer();
	m_microsecunds = dGetTimeInMicrosenconds ();
}

void DemoEntityManager::UpdateCamera (float timestep)
{
	// somehow this does not works with QT
	//GetKeyAsynState(100);

	m_cannonBallRate -= int (timestep * 1000000.0f);
	Lock (m_navegationQueueLock);
	{
		newtonDemos* const mainWindow = (newtonDemos*) parent();
		
		float sideSpeed = mainWindow->m_sidewaysSpeed;
		float linearSpeed = mainWindow->m_cameraFrontSpeed;
		dMatrix targetMatrix (m_camera->GetNextMatrix());
		for (int i = 0; i < m_navegationQueueCount; i ++) {

			int code = m_navegationQueue[i];
			switch (code)
			{
				case _moveForward:
				{
					targetMatrix.m_posit += targetMatrix.m_front.Scale(linearSpeed * timestep);
					break;
				}

				case _moveBackward:
				{
					targetMatrix.m_posit -= targetMatrix.m_front.Scale(linearSpeed * timestep);
					break;
				}

				case _moveLeft:
				{
					targetMatrix.m_posit -= targetMatrix.m_right.Scale(sideSpeed * timestep);
					break;
				}

				case _moveRight:
				{
					targetMatrix.m_posit += targetMatrix.m_right.Scale(sideSpeed * timestep);
					break;
				}

				case _mouseMove:
				{
					if (mainWindow->m_mouseSpeedX > 0) {
						m_cameraYaw = dMod(m_cameraYaw + mainWindow->m_yawRate, 2.0f * 3.1416f);
					} else if (mainWindow->m_mouseSpeedX < 0){
						m_cameraYaw = dMod(m_cameraYaw - mainWindow->m_yawRate, 2.0f * 3.1416f);
					}
					if (mainWindow->m_mouseSpeedY > 0) {
						m_cameraPitch += mainWindow->m_pitchRate;
					} else if (mainWindow->m_mouseSpeedY < 0){
						m_cameraPitch -= mainWindow->m_pitchRate;
					}
					if (m_cameraPitch > 80.0f * 3.1416f / 180.0f) {
						m_cameraPitch = 80.0f * 3.1416f / 180.0f;
					} else if (m_cameraPitch < -80.0f * 3.1416f / 180.0f) {
						m_cameraPitch = -80.0f * 3.1416f / 180.0f;
					}
					break;
				}

				case _shotCannonBall:
				{
					 if (m_cannonBallRate <= 0) {
						m_cannonBallRate = int (0.25f * 1000000.0f);

						if (!m_meshBallMesh) {
							NewtonCollision* const ball = NewtonCreateSphere (m_world, 0.25f, 0.25f, 0.25f, 0, NULL);
							m_meshBallMesh = new DemoMesh("ball", ball, "base_road.tga", "base_road.tga", "base_road.tga");
							NewtonReleaseCollision(m_world, ball);
						}

						dMatrix matrix (GetIdentityMatrix());
						matrix.m_posit = targetMatrix.m_posit;

						dVector veloc (targetMatrix.m_front.Scale (PROJECTILE_INITIAL_SPEED));
						NewtonCollision* const ballCollision = NewtonCreateSphere (m_world, 0.25f, 0.25f, 0.25f, 0, NULL);
						NewtonBody* const body = CreateSimpleSolid (this, m_meshBallMesh, 100.0f, matrix, ballCollision, 0);
						NewtonReleaseCollision(m_world, ballCollision);

						NewtonBodySetVelocity(body, &veloc[0]);
					}

					break;
				}
				
			}			

 		}

		dMatrix matrix (dRollMatrix(m_cameraPitch) * dYawMatrix(m_cameraYaw));
		dQuaternion rot (matrix);
		m_camera->SetMatrix (*this, rot, targetMatrix.m_posit);
		m_navegationQueueCount = 0;
	}
	Unlock (m_navegationQueueLock);
}

void DemoEntityManager::UpdatePhysics()
{
	// read the controls 


	// update the physics
	if (m_world) {
		dFloat timestepInSecunds = 1.0f / MAX_PHYSICS_FPS;
		unsigned64 timestepMicrosecunds = unsigned64 (timestepInSecunds * 1000000.0f);

		unsigned64 currentTime = dGetTimeInMicrosenconds ();
		unsigned64 nextTime = currentTime - m_microsecunds;
		int loops = 0;
		while ((nextTime >= timestepMicrosecunds) && (loops < MAX_PHYSICS_LOOPS)) {
			loops ++;

			// run the newton update function
			if (!m_reEntrantUpdate) {
				m_reEntrantUpdate = true;
				if (m_physicsUpdate && m_world) {
					// update the camera;
					UpdateCamera (timestepInSecunds);
					// update teh world
					NewtonUpdate (m_world, timestepInSecunds);

					dListNode* nextNode;
					for (dListNode* node = GetFirst(); node; node = nextNode) {
						nextNode = node->GetNext();

						DemoEntity* const entity = node->GetInfo();
						entity->AddRef();
						entity->SimulationLister(this, node, timestepInSecunds);
						entity->Release();
					}

					
				}
				m_reEntrantUpdate = false;
			}

			nextTime -= timestepMicrosecunds;
			m_microsecunds += timestepMicrosecunds;
		}

		if (loops) {
			m_physicsTime = dFloat (dGetTimeInMicrosenconds () - currentTime) / 1000000.0f;

			if (m_physicsTime >= MAX_PHYSICS_LOOPS * (1.0f / MAX_PHYSICS_FPS)) {
				m_microsecunds = currentTime;
			}
		}
	}
}

void DemoEntityManager::QueueCommand(int command)
{
	Lock (m_navegationQueueLock);
	_ASSERTE (m_navegationQueueCount < int (sizeof (m_navegationQueue) / sizeof (m_navegationQueue[0])));
	if (m_navegationQueueCount < int (sizeof (m_navegationQueue) / sizeof (m_navegationQueue[0]))) {
		m_navegationQueue[m_navegationQueueCount] = command;
		m_navegationQueueCount ++;
	}
	Unlock (m_navegationQueueLock);
}

void DemoEntityManager::paintEvent(QPaintEvent* ev)
{
	newtonDemos* const mainWindow = (newtonDemos*) parent();

	if (!m_asycronousUpdate) {
		UpdatePhysics();
	}

	makeCurrent();

	InterpolateMatrices ();

	// Our shading model--Goraud (smooth). 
	glShadeModel (GL_SMOOTH);

	// Culling. 
	glCullFace (GL_BACK);
	glFrontFace (GL_CCW);
	glEnable (GL_CULL_FACE);

	//	glEnable(GL_DITHER);

	// z buffer test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	glClearColor (0.5f, 0.5f, 0.5f, 0.0f );
	//glClear( GL_COLOR_BUFFER_BIT );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	// set default lightning
	glDisable(GL_BLEND);
	glEnable (GL_LIGHTING);
	dFloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// set just one directional light
	GLfloat lightColor[] = { 0.8f, 0.8f, 0.8f, 0.0 };
	GLfloat lightAmbientColor[] = { 0.3f, 0.3f, 0.3f, 0.0 };
	GLfloat lightPosition[] = { -500.0f, 200.0f, 500.0f, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	glEnable(GL_LIGHT0);

	// update Camera
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	m_camera->SetProjectionMode(width(), height());
	m_camera->Update();

	// render all entities
	dFloat timestep = dGetElapsedSeconds();	
	for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
		DemoEntity* const entity = node->GetInfo();
		glPushMatrix();	
		entity->Render(timestep);
		glPopMatrix();
	}

	if (mainWindow->m_showContactPointState) {
		RenderContactPoints (GetNewton());
	}

	if (mainWindow->m_debugDisplayState) {
		DebugRenderWorldCollision (GetNewton());
	}

	// do all 2d drawing
	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	QPainter painter(this);
	//QFontMetrics metrics = QFontMetrics(m_font);
	//int border = qMax(4, metrics.leading());
	//painter.setRenderHint(QPainter::Antialiasing);
	//renderText (4, 4, "This is a test", m_font);
	if (mainWindow->m_physicProfilerState) {
		int profileFlags = 0;
		for (int i = 0; i < int (sizeof (m_showProfiler) / sizeof (m_showProfiler[0])); i ++) {
			profileFlags |=  m_showProfiler[i] ? (1 << i) : 0;
		}

		//profileFlags = 1;
		if (profileFlags) {
			m_profiler.Render (m_world, profileFlags, painter);
		}
	}

	if (mainWindow->m_threadProfilerState) {
		m_profiler.RenderThreadPerformance (m_world, painter);
	}

	if (mainWindow->m_showStatistics) {
		dFloat fps = 1.0f / timestep;
		painter.setPen(Qt::black);
		Print (painter, 14, 14, "FPS: %6.2f", fps);
		Print (painter, 14, 30, "Physics time (ms): %6.3f", m_physicsTime * 1000.0f);
		Print (painter, 14, 46, "Body count: %d",  NewtonWorldGetBodyCount(m_world));
		Print (painter, 14, 62, "number of threads: %d",  NewtonGetThreadsCount(m_world));
		if (m_asycronousUpdate) {
			Print (painter, 14, 78, "physics running asynchronous");
		}
	}
	painter.end();
}

void DemoEntityManager::timerEvent(QTimerEvent *)
{
	update();
}

