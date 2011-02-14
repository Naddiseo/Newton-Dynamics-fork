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

#include "toolbox_stdafx.h"
#include "sdkDemos.h"
#include "NewtonDemos.h"
#include "DemoEntityManager.h"

//void BasicSphereSpin (DemoEntityManager* scene);
void Friction (DemoEntityManager* const scene);
void ClosestDistance (DemoEntityManager* const scene);
void BasicBoxStacks (DemoEntityManager* const scene);
void SimpleMeshLevelCollision (DemoEntityManager* const scene);
void OptimizedMeshLevelCollision (DemoEntityManager* const scene);
void SceneCollision (DemoEntityManager* const scene);

static SDKDemos demosSelection[] = 
{
	{"coefficients of friction", "demonstrate the effect of various coefficient of friction", Friction},
	{"closest distance", "demonstrate closest distance to a convex shape", ClosestDistance},
	{"Simple Box Stacks", "show simple stack of Boxes", BasicBoxStacks},
	{"Unoptimized Mesh collision", "show simple level mesh", SimpleMeshLevelCollision},
	{"Optimized Mesh collision", "show optimized level mesh", OptimizedMeshLevelCollision},
	{"multi geometry collision", "show static mesh with teh avility of moving internal parts", SceneCollision},


/*
	{"spinning sphere", "show a very simple rotating sphere", BasicSphereSpin},
	{"Precessing tops", "show natural precession", PrecessingTops},
	{"convex cast", "demonstrate convex cast different shapes", ConvexCast},
	{"basic box stacking", "demonstrate box stacking", BasicBoxStacks},

	
	{"coefficients of restitution", "demonstrate the effect of various coefficient of restitution", Restitution},
	{"basic cylinder stacking", "demonstrate cylinder stacking", BasicCylinderStacks},
	{"basic convex hull stacking", "demonstrate convex hull stacking", BasicConvexStacks},
	{"basic unstable stacking", "demonstrate stability stacking unstable objects", UnstableStacks},
	{"UnstableStacks stacking", "demonstrate stability stacking unstable objects", UnstableStruture},
	{"Jenga stacking", "demonstrate Jenga game", Jenga},
	{"Large Jenga stacking", "demonstrate Jenga game", JengaTall},
	{"small pyramid stacking", "demonstrate small pyramid stacking", CreatePyramid},
	{"large pyramid stacking", "demonstrate small pyramid stacking", CreatePyramidTall},
	{"wall stacking", "demonstrate wall stacking", CreateWalls},
	{"small tower stacking", "demonstrate tower stacking", CreateTower},
	{"large tower stacking", "demonstrate tower stacking", CreateTowerTall},
	{"scaled collision shape", "demonstrate scaling shape", ScaledCollision},
	{"compound collision shape", "demonstrate compound collision", CompoundCollision},
	{"convex approximation shape", "demonstrate convex approximation of close complex meshes", ConvexApproximation},
	{"simple Plane collision", "demonstrate simple Plane collision", SimplePlaneCollision},
	{"simple polygon mesh collision", "demonstrate simple polygon mesh collision", SimpleMeshLevelCollision},
	{"optimized polygon mesh collision", "demonstrate optimized polygon mesh collision", OptimizedMeshLevelCollision},
	{"simple height field collision", "demonstrate simple hight field collision", SimpleHeighMapCollision},
	{"user defined polygon static collision", "demonstrate user defined polygon static collision", UserHeighMapColliion},
	{"multi shape static level collision", "demonstrate multi shape static level collision", SceneCollision},
	{"attractive magnets force field", "demonstrate attractive force field", Magnets},
	{"repulsive magnets force field", "demonstrate repulsive magnet force field", Repulsive},
	{"Archimedes buoyancy force field", "demonstrate user define Archimedes as force field", ArchimedesBuoyancy},
	{"legacy joints", "demonstrate the build in joints", LegacyJoints},
	{"custom joints", "demonstrate custom joints", BasicCustomJoints},
	{"Simple robots", "demonstrate custom joints robot", BasicRobots},
	{"motorized robots", "demonstrate motorized custom joints robot", TracktionJoints},
	//	{"raycast car", "demonstrate simple ray cast vehicle joint", RayCastCar},
	{"discrete rag doll", "demonstrate simple rag doll", DescreteRagDoll},
	{"skinned rag doll", "demonstrate simple rag doll", SkinRagDoll},
*/
	//	{"player controller", "demonstrate simple player joint", PlayerController},
};


#define ID_FILE_EXIT__ 9001
#define ID_STUFF_GO__ 9002

void LoadMenuSeletions (HWND mainWindow)
{
	HMENU hMenu = GetMenu(mainWindow);  
	int count = GetMenuItemCount(hMenu);
	for (int i = 0; i < count; i ++) {
		char name[256];
		GetMenuString(hMenu, i, name, sizeof (name), MF_BYPOSITION);
		if (!stricmp (name, "demos")) {
			hMenu = GetSubMenu(hMenu, i);
			break;
		}
	}

	DeleteMenu(hMenu, 0, MF_BYPOSITION);
	for (int i = 0; i < sizeof (demosSelection) / sizeof demosSelection[0]; i ++) {
		AppendMenu(hMenu, MF_STRING, IDM_DEMOS + i, demosSelection[i].m_name);
	}
}


void LoadDemo (DemoEntityManager* scene, int menuEntry)
{
	demosSelection[menuEntry].m_launchDemoCallback (scene);
	scene->SetAutoSleepState (autoSleepState);
}