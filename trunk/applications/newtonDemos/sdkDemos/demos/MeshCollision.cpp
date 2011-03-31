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

#include <toolbox_stdafx.h>
#include "SkyBox.h"
#include "../DemoMesh.h"
#include "../DemoEntityManager.h"
#include "../DemoCamera.h"
#include "../PhysicsUtils.h"



#if 0
static void SetDemoCallbacks (NewtonFrame& system)
{
	system.m_control = Keyboard;
	system.m_autoSleep = AutoSleep;
	system.m_showIslands = SetShowIslands;
	system.m_showContacts = SetShowContacts; 
	system.m_setMeshCollision = SetShowMeshCollision;
}

static LevelPrimitive* LoadLevelAndSceneRoot (NewtonFrame& system, const char* levelName, int optimized)
{
_ASSERTE (0);
return NULL;
/*

	NewtonWorld* world;
	NewtonBody* floorBody;
	LevelPrimitive* level;

	world = system.m_world;
	// /////////////////////////////////////////////////////////////////////
	//
	// create the sky box,
	OGLModel* sky = new SkyBox ();
	system.AddModel___ (sky);
	sky->Release();


	// Load a level geometry

	level = new LevelPrimitive (levelName, world, optimized);


	system.AddModel___(level);
	level->Release();
	floorBody = level->m_level;

	// set a destructor for this rigid body
	NewtonBodySetDestructorCallback (floorBody, PhysicsBodyDestructor);

	// get the default material ID
	int defaultID;
	defaultID = NewtonMaterialGetDefaultGroupID (world);

	// set default material properties
	NewtonMaterialSetDefaultSoftness (world, defaultID, defaultID, 0.05f);
	NewtonMaterialSetDefaultElasticity (world, defaultID, defaultID, 0.4f);
	NewtonMaterialSetDefaultCollidable (world, defaultID, defaultID, 1);
	NewtonMaterialSetDefaultFriction (world, defaultID, defaultID, 1.0f, 0.5f);
	NewtonMaterialSetCollisionCallback (world, defaultID, defaultID, NULL, NULL, GenericContactProcess); 

//	NewtonMaterialSetSurfaceThickness(world, materialID, materialID, 0.1f);
//	NewtonMaterialSetSurfaceThickness(world, defaultID, defaultID, 0.0f);

	// set the island update callback
	NewtonSetIslandUpdateEvent (world, PhysicsIslandUpdate);

	// save th3 callback
	SetDemoCallbacks (system);

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), dVector (-40.0f, 10.0f, 0.0f));

	return level;
*/
}




static void AddBoxes(SceneManager* system, dFloat mass, const dVector& origin, const dVector& oriSize, int xCount, int zCount, dFloat spacing, PrimitiveType type, int materialID)
{
_ASSERTE (0);
/*

	dVector size (oriSize);
	dMatrix matrix (GetIdentityMatrix());

	// create the shape and visual mesh as a common data to be re used
//	type = _BOX_PRIMITIVE;
	NewtonCollision* boxCollision = CreateConvexCollision (system->m_world, GetIdentityMatrix(), size, type, materialID);

	char name[256];
	sprintf (name, "shape%d", type);
	OGLMesh* boxMesh = new OGLMesh (name, boxCollision, "wood_0.tga", "wood_0.tga", "wood_1.tga");

	for (int i = 0; i < xCount; i ++) {
		dFloat x;
		x = origin.m_x + (i - xCount / 2) * spacing;

		for (int j = 0; j < zCount; j ++) {
			dFloat z;
			z = origin.m_z + (j - zCount / 2) * spacing;

			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (system->m_world, x, z) + 4.0f;
			matrix.m_posit.m_z = z;
//			CreateGenericSolid (system->m_world, system, mass, matrix, size, type, materialID);
			CreateSimpleSolid (system->m_world, system, boxMesh, mass, matrix, boxCollision, materialID);
		}
	}

	// do not forget to release the assets	
	boxMesh->Release(); 
	NewtonReleaseCollision(system->m_world, boxCollision);
*/
}





void SimplePlaneCollision (NewtonFrame& system)
{
	NewtonWorld* world;
	LevelPrimitive *level;
	NewtonCollision* planeColl;

	world = system.m_world;

	// create a material carrier to collision with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	// create the sky box and the floor,
	level = LoadLevelAndSceneRoot (system, "flatplane.dae", 1);

	// Find the world mesh and replace the collision for a custom plane collision mesh
	dMatrix matrix;
	NewtonBodyGetMatrix (level->m_level, &matrix[0][0]);
	dVector plane (matrix.m_up);
	plane.m_w = - (plane % matrix.m_posit);
	planeColl = CreatePlaneCollidion (world, plane);
	NewtonBodySetCollision(level->m_level, planeColl);
	NewtonReleaseCollision(world, planeColl);


	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes (&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}


void SimpleHeighMapCollision (NewtonFrame& system)
{
_ASSERTE (0);
/*

	NewtonWorld* world;
	LevelPrimitive *level;
	HeightFieldPrimitive* map;

	world = system.m_world;

	// create the sky box and the floor,
	level = LoadLevelAndSceneRoot (system, "flatplane.dae", 0);

	// create a material to collide with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	// delete the existing levelMesh
	NewtonDestroyBody (world, level->m_level);
	system.RemoveModel (level);

	// add a height map mesh 
	map = new HeightFieldPrimitive (world);
	system.AddModel___ (map);
	map->Release();


	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
*/
}

void UserHeighMapColliion (NewtonFrame& system)
{
_ASSERTE (0);
/*

	NewtonWorld* world;
	LevelPrimitive *level;
	UserHeightFieldCollision* map;

	world = system.m_world;

	// create the sky box and the floor,
	level = LoadLevelAndSceneRoot (system, "flatplane.dae", 0);

	// delete the existing levelMesh
	NewtonDestroyBody (world, level->m_level);
	system.RemoveModel (level);


	// add a height map mesh 
	map = new UserHeightFieldCollision (world);
	system.AddModel___ (map);
	map->Release();

	// create a material to collision with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);


	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
*/
}




void OptimizedMeshLevelCollision (NewtonFrame& system)
{
	NewtonWorld* world;

	world = system.m_world;

	// create the sky box and the floor,
	//LoadLevelAndSceneRoot (system, "flatplane.dae", 1);
	//LoadLevelAndSceneRoot (system, "dungeon.dae", 1);
	//LoadLevelAndSceneRoot (system, "pitpool.dae", 1);
	LoadLevelAndSceneRoot (system, "pitpool.dae", 1);
	//LoadLevelAndSceneRoot (system, "dungeon.dae", 1);


	// create a material to collide with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;

	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}


void SimpleMeshLevelCollision (NewtonFrame& system)
{
	NewtonWorld* world;

	world = system.m_world;

	// create the sky box and the floor,
//	LoadLevelAndSceneRoot (system, "flatplane.dae", 0);
//	LoadLevelAndSceneRoot (system, "dungeon.dae", 0);
//	LoadLevelAndSceneRoot (system, "xxx.dae", 1);
//	LoadLevelAndSceneRoot (system, "xxx1.dae", 1);
//	LoadLevelAndSceneRoot (system, "pitpool1.dae", 0);
	LoadLevelAndSceneRoot (system, "pitpool.dae", 0);
//	LoadLevelAndSceneRoot (system, "playground.dae", 0);
	

	// create a material to collide with this object
	int defaultMaterialID;
	defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);

	dVector posit (0.0f, 0.0f, 0.0f, 0.0f);
	posit.m_y = FindFloor (world, posit.m_x, posit.m_z) + 5.0f;


	InitEyePoint (dVector (1.0f, 0.0f, 0.0f), posit);

	dVector size (1.0f, 1.0f, 1.0f);
	dVector location (cameraEyepoint + cameraDir.Scale (10.0f));

	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _BOX_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CONE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddBoxes(&system, 10.0f, location, size, 3, 3, 10.0f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
}
#endif




static void SimpleMeshLevel (DemoEntityManager* const scene, bool optimization)
{
	// suspend simulation before making changes to the physics world
	scene->StopsExecution ();

	// load the skybox
	scene->Append(new SkyBox());


	// load the scene from and alchemedia file format
//	CreateLevelMesh (scene, "flatPlane.xml", optimization);
	CreateLevelMesh (scene, "sponza.xml", optimization);
//	CreateLevelMesh (scene, "cattle.xml", fileName);
//	CreateLevelMesh (scene, "playground.xml", fileName);
	
	// place camera into position
//	dQuaternion rot;
//	dVector posit (0.0f, 1.0f, 0.0f, 0.0f);
//	scene->SetCameraMatrix(rot, posit);

	dMatrix camMatrix (dRollMatrix(-20.0f * 3.1416f /180.0f) * dYawMatrix(-45.0f * 3.1416f /180.0f));
	dQuaternion rot (camMatrix);
//dQuaternion rot;
	dVector origin (-30.0f, 40.0f, -15.0f, 0.0f);
//	dVector origin (-10.0f, 1.0f, 0.0f, 0.0f);
	scene->SetCameraMatrix(rot, origin);

	NewtonWorld* const world = scene->GetNewton();
	int defaultMaterialID = NewtonMaterialGetDefaultGroupID (world);
	dVector location (0.0f, 0.0f, 0.0f, 0.0f);
	dVector size (0.5f, 0.5f, 0.5f, 0.0f);
	int count = 8;
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _SPHERE_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _BOX_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _CYLINDER_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _CHAMFER_CYLINDER_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _BOX_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _CONE_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _CAPSULE_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
	AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);

//for (int i = 0; i < 20; i ++)
//AddPrimitiveArray(scene, 10.0f, location, size, count, count, 1.7f, _SPHERE_PRIMITIVE, defaultMaterialID);

	// resume the simulation
	scene->ContinueExecution();
}

void OptimizedMeshLevelCollision (DemoEntityManager* const scene)
{
	SimpleMeshLevel (scene, true);
}

void SimpleMeshLevelCollision (DemoEntityManager* const scene)
{
	SimpleMeshLevel (scene, false);
}