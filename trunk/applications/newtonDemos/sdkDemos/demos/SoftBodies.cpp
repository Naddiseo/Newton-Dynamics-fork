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



static NewtonCollision* CreateSoftBodyCollisionShape (DemoEntityManager* const scene)
{

	NewtonWorld* const world = scene->GetNewton();

	// create two auxiliary objects to help with the graphics part
	// make a Box collision as a source to make a mesh 
	dVector size (1.0f, 1.0f, 1.0f, 0.0f);

//	NewtonCollision* const box = CreateConvexCollision (world, GetIdentityMatrix(), size, _BOX_PRIMITIVE, 0);
	NewtonCollision* const box = CreateConvexCollision (world, GetIdentityMatrix(), size, _SPHERE_PRIMITIVE, 0);

	// now convert the collision into a mesh, with materials and UV
	NewtonMesh* const mesh = NewtonMeshCreateFromCollision(box);
	int material = LoadTexture("smilli.tga");
	NewtonMeshApplyBoxMapping(mesh, material, material, material);


	// now create a soft collision mesh
	NewtonCollision* const softCollisionMesh = NewtonCreateDeformableMesh (world, mesh, 0);
//	NewtonCollision* const softCollisionMesh = CreateConvexCollision (world, GetIdentityMatrix(), size, _SPHERE_PRIMITIVE, 0);

	// destroy the auxiliary objects
	NewtonMeshDestroy(mesh);
	NewtonReleaseCollision(world, box);

	return softCollisionMesh;
}


static NewtonBody* CreateSoftBody (DemoEntityManager* const scene, NewtonCollision* const softCollision)
{
	NewtonWorld* const world = scene->GetNewton();
	DemoMesh* const mesh = new DemoMesh("mesh", softCollision, "smilli.tga", "smilli.tga", "smilli.tga");
	_ASSERTE (mesh);

	// add an new entity to the world
	DemoEntity* const entity = new DemoEntity(NULL);
	scene->Append (entity);
	entity->SetMesh(mesh);
	mesh->Release();

	// calculate the moment of inertia and the relative center of mass of the solid
//	dVector origin;
//	dVector inertia;
//	NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	
//	dFloat Ixx = mass * inertia[0];
//	dFloat Iyy = mass * inertia[1];
//	dFloat Izz = mass * inertia[2];

	//create the rigid body
	dMatrix matrix (GetIdentityMatrix());
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	matrix.m_posit.m_x = x;
	matrix.m_posit.m_z = y;
	matrix.m_posit.m_y = FindFloor (world, x, z) + 4.0f;
	entity->SetMatrix(*scene, dQuaternion(), matrix.m_posit);
	entity->SetMatrix(*scene, dQuaternion(), matrix.m_posit);

//	NewtonBody* const deformableBody = NewtonCreateBody (world, softCollision, &matrix[0][0]);
	NewtonBody* const deformableBody = NewtonCreateDeformableBody (world, softCollision, &matrix[0][0]);

	// set the correct center of gravity for this body
//	NewtonBodySetCentreOfMass (deformableBody, &origin[0]);

	// set the mass matrix
//	NewtonBodySetMassMatrix (deformableBody, mass, Ixx, Iyy, Izz);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (deformableBody, entity);

	// assign the wood id
//	NewtonBodySetMaterialGroupID (deformableBody, materialId);

	// set a destructor for this rigid body
	NewtonBodySetDestructorCallback (deformableBody, PhysicsBodyDestructor);

	// set the transform call back function
	NewtonBodySetTransformCallback (deformableBody, DemoEntity::SetTransformCallback);

	// set the force and torque call back function
	NewtonBodySetForceAndTorqueCallback (deformableBody, PhysicsApplyGravityForce);

	return deformableBody;
}


void SoftBodies(DemoEntityManager* const scene)
{
	// suspend simulation before making changes to the physics world
	scene->StopsExecution ();

	// load the skybox
	scene->Append(new SkyBox());


	// load the scene from and alchemedia file format
	CreateLevelMesh (scene, "flatPlane.ngd", 1);


	NewtonWorld* const world = scene->GetNewton();
	NewtonCollision* const collision = CreateSoftBodyCollisionShape (scene);
	NewtonBody* const body = CreateSoftBody (scene, collision);

	NewtonReleaseCollision(world, collision);

	dQuaternion rot;
	dVector origin (-10.0f, 2.0f, 0.0f, 0.0f);
	scene->SetCameraMatrix(rot, origin);

//	scene->SaveScene ("test1.ngd");
//	dScene CreateAlchemediaFromPhysic(); 

	// resume the simulation
	scene->ContinueExecution();
}

