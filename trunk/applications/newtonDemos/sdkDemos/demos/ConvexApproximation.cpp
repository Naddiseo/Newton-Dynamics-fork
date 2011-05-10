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
#include "TargaToOpenGl.h"
#include "../DemoMesh.h"
#include "../DemoEntityManager.h"
#include "../DemoCamera.h"
#include "../PhysicsUtils.h"

#define INITIAL_DELAY				1000
//#define INITIAL_DELAY				0
#define NUMBER_OF_ITERNAL_PARTS		10
#define BREAK_FORCE_IN_GRAVITIES	10
//#define BREAK_FORCE_IN_GRAVITIES	1

#if 0
static void TestConvexApproximation (DemoEntityManager* const scene)
{
	// create a collision primitive
//	dVector size (2.0f, 2.0f, 2.0f);
//	dVector size = dVector (10.0f, 0.5f, 10.0f, 0.0f);
	dVector size = dVector (4.0f, 4.0f, 4.0f, 0.0f);
	NewtonWorld* const world = scene->GetNewton();

	NewtonCollision* const collision = CreateConvexCollision (world, GetIdentityMatrix(), size, _BOX_PRIMITIVE, 0);
//	NewtonCollision* const collision = CreateConvexCollision (world, GetIdentityMatrix(), size, _SPHERE_PRIMITIVE, 0);
//	NewtonCollision* const collision = CreateConvexCollision (world, GetIdentityMatrix(), size, _REGULAR_CONVEX_HULL_PRIMITIVE, 0);
//	NewtonCollision* const collision = CreateConvexCollision (world, GetIdentityMatrix(), size, _RANDOM_CONVEX_HULL_PRIMITIVE, 0);
	
	// create a newton mesh from the collision primitive
	NewtonMesh* const mesh = NewtonMeshCreateFromCollision(collision);

	// apply a simple Box Mapping
	int tex0 = LoadTexture("reljef.tga");
	NewtonMeshApplyBoxMapping(mesh, tex0, tex0, tex0);


	// create a thing box;
	dVector size1 = dVector (size.m_x * 4.0f, size.m_y * 0.5f, size.m_z * 0.5f, 0.0f);
//	dVector size1 = dVector (size.m_x * 0.5f, size.m_y * 0.5f, size.m_z * 0.5f, 0.0f);
	NewtonCollision* const collision1 = CreateConvexCollision (world, GetIdentityMatrix(), size1, _BOX_PRIMITIVE, 0);
	NewtonMesh* const brush = NewtonMeshCreateFromCollision(collision1);

	int interior = LoadTexture("KAMEN-stup.tga");
	NewtonMeshApplyBoxMapping(brush, interior, interior, interior);


	// now use th brush to carve the big box
//	dMatrix matrix (GetIdentityMatrix());
	dMatrix matrix (dPitchMatrix(3.14159265f * 0.5f / 180.0f));
//	matrix.m_posit.m_x = -(size.m_x - size1.m_x) * 0.5f;
//	matrix.m_posit.m_y = -(size.m_y) * 0.5f;
//	NewtonMesh* const mesh1 = NewtonMeshUnion(mesh, brush, &matrix[0][0]);
//	NewtonMesh* const mesh1 = NewtonMeshDifference (brush, mesh, &matrix[0][0]);

	NewtonMesh* const mesh1 = NewtonMeshDifference (mesh, brush, &matrix[0][0]);

	matrix = dRollMatrix (3.14159265f * 0.5f);
	NewtonMesh* const mesh2 = NewtonMeshDifference (mesh1, brush, &matrix[0][0]);

	matrix = dYawMatrix( 3.14159265f * 0.5f);
	NewtonMesh* const mesh3 = NewtonMeshDifference (mesh2, brush, &matrix[0][0]);

	dMatrix textureMatrix (GetIdentityMatrix());
	textureMatrix[0][0] = 1.0f / size.m_x;
	textureMatrix[1][1] = 1.0f / size.m_y;

//	NewtonMesh* const tetrahedras = NewtonMeshTetrahedralization (mesh1, interior, &textureMatrix[0][0]);
//	NewtonMesh* const tetrahedras = NewtonMeshTetrahedralization (mesh2, interior, &textureMatrix[0][0]);
//	NewtonMesh* const tetrahedras = NewtonMeshTetrahedralization (mesh3, interior, &textureMatrix[0][0]);
	


#if 1
dScene xxxx(world);
dScene::dTreeNode* const modelNode = xxxx.CreateSceneNode(xxxx.GetRootNode());
dScene::dTreeNode* const meshNode = xxxx.CreateMeshNode(modelNode);
dMeshNodeInfo* const modelMesh = (dMeshNodeInfo*)xxxx.GetInfoFromNode(meshNode);
//modelMesh->ReplaceMesh (mesh);
//modelMesh->ReplaceMesh (brush);
//modelMesh->ReplaceMesh (mesh1);
modelMesh->ReplaceMesh (mesh2);
//modelMesh->ReplaceMesh (mesh3);
//modelMesh->ReplaceMesh (tetrahedras);

xxxx.Serialize("../../../media/xxx.ngd");
#endif

/*
	DemoEntity* const entity = new DemoEntity(NULL);
	entity->SetMatrix(*scene, dQuaternion(), dVector (0, 10, 0, 0));
	entity->InterpolateMatrix (*scene, 1.0f);
	
	
	scene->Append (entity);
	DemoMesh* const mesh1 = new DemoMesh(convexParts);
	entity->SetMesh(mesh1);
	mesh1->Release();


//DemoEntity* const entity2 = new DemoEntity(NULL);
//entity2->SetMatrix(*scene, dQuaternion(), dVector (0, 10, 0, 0));
//entity2->InterpolateMatrix (*scene, 1.0f);

//scene->Append (entity2);
//DemoMesh* const mesh2 = new DemoMesh(mesh);
//entity2->SetMesh(mesh2);
//mesh2->Release();


	// make sure the assets are released before leaving the function
	if (convexParts) {
		NewtonMeshDestroy (convexParts);
	}
	NewtonMeshDestroy (mesh);
	NewtonReleaseCollision(world, collision);
*/
}
#endif



static void LoadCompoundModel (const char* const name, DemoEntityManager* const scene, const dVector& origin)
{
	char fileName[2048];
	GetWorkingFileName (name, fileName);

	NewtonWorld* const world = scene->GetNewton();
	dScene compoundTestMesh (world);
	compoundTestMesh.Deserialize(fileName);

	dMeshNodeInfo* mesh = NULL;
	for (dScene::dTreeNode* node = compoundTestMesh.GetFirstNode (); node; node = compoundTestMesh.GetNextNode (node)) {
		dNodeInfo* info = compoundTestMesh.GetInfoFromNode(node);
		if (info->GetTypeId() == dMeshNodeInfo::GetRttiType()) {
			mesh = (dMeshNodeInfo*) info;
			break;
		}
	}

	_ASSERTE (mesh);
	_ASSERTE (mesh->GetMesh());

	NewtonMesh* const convexDocomposition = NewtonMeshApproximateConvexDecomposition (mesh->GetMesh());

mesh->ReplaceMesh (convexDocomposition);
compoundTestMesh.Serialize("../../../media/xxx1.ngd");


		

	NewtonMeshDestroy (convexDocomposition);
}




void SimpleConvexAproximation (DemoEntityManager* const scene)
{
	// suspend simulation before making changes to the physics world
	scene->StopsExecution ();

	// load the skybox
	scene->Append(new SkyBox());

	// load the scene from and alchemedia file format
	CreateLevelMesh (scene, "flatPlane.ngd", false);
//	CreateLevelMesh (scene, "sponza.ngd", false);

	// create a shattered mesh array
//    TestConvexApproximation (scene);


//	int defaultMaterialID = NewtonMaterialGetDefaultGroupID (scene->GetNewton());
	dVector location (0.0f, 0.0f, 0.0f, 0.0f);
	dVector size (0.5f, 0.5f, 0.5f, 0.0f);
//	int count = 5;
//	LoadCompoundModel ("box_1.ngd", scene, location);
//	LoadCompoundModel ("xxx.ngd", scene, location);
//	LoadCompoundModel ("lshape.ngd", scene, location);
//	LoadCompoundModel ("box_2.ngd", scene, location);
//	AddShatterPrimitive(scene, 10.0f, location, size, count, count, 1.7f, _BOX_PRIMITIVE, defaultMaterialID);
//	AddShatterPrimitive(scene, 10.0f, location, size, count, count, 1.7f, _REGULAR_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
//	AddShatterPrimitive(scene, 10.0f, location, size, count, count, 1.7f, _RANDOM_CONVEX_HULL_PRIMITIVE, defaultMaterialID);
//	AddShatterPrimitive(scene, 10.0f, location, size, count, count, 1.7f, _SPHERE_PRIMITIVE, defaultMaterialID);
//	AddShatterPrimitive(scene, 10.0f, location, size, count, count, 1.7f, _CYLINDER_PRIMITIVE, defaultMaterialID);
//	AddShatterPrimitive(scene, 10.0f, location, size, count, count, 1.7f, _CONE_PRIMITIVE, defaultMaterialID);
//	AddShatterPrimitive(scene, 10.0f, location, size, count, count, 1.7f, _CAPSULE_PRIMITIVE, defaultMaterialID);


	// place camera into position
	dQuaternion rot;
//	dVector origin (-40.0f, 10.0f, 0.0f, 0.0f);
	dVector origin (-15.0f, 10.0f, 0.0f, 0.0f);
	scene->SetCameraMatrix(rot, origin);

	// resume the simulation
	scene->ContinueExecution();
}


