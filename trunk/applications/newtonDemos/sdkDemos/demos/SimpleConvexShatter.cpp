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

#define NUMBER_OF_ITERNAL_PARTS 5

#if 0
static void CreateSimpleVoronoiShatter (DemoEntityManager* const scene)
{
	// create a collision primitive
//	dVector size (2.0f, 2.0f, 2.0f);
	dVector size = dVector (10.0f, 0.5f, 10.0f, 0.0f);
	NewtonWorld* const world = scene->GetNewton();
//	NewtonCollision* const collision = CreateConvexCollision (world, GetIdentityMatrix(), size, type, 0);
	NewtonCollision* const collision = CreateConvexCollision (world, GetIdentityMatrix(), size, _BOX_PRIMITIVE, 0);
	

	// create a newton mesh from the collision primitive
	NewtonMesh* const mesh = NewtonMeshCreateFromCollision(collision);

	// apply a simple Box Mapping
	int tex0 = LoadTexture("reljef.tga");
	NewtonMeshApplyBoxMapping(mesh, tex0, tex0, tex0);

	// pepper the bing box of the mesh with random points
	int count = 0;
	dVector points[10];
	do {
		dFloat x = RandomVariable(size.m_x);
		dFloat y = RandomVariable(size.m_y);
		dFloat z = RandomVariable(size.m_z);
		if ((x <= size.m_x) && (x >= -size.m_x) && (y <= size.m_y) && (y >= -size.m_y) && (z <= size.m_z) && (z >= -size.m_z)){
			points[count] = dVector (x, y, z);
			count ++;
		}
	} while (count < 10);

//points[0] =	dVector (-0.95387375f, 0.051684085f, -1.3675097f);
//points[1] = dVector (0.027149497f, -0.038374674f, 0.47796631f);
//points[2] = dVector (0.24927004f, -0.015358878f, -0.45922884f);
//count = 3;


//count = 0;

	// Create the array of convex pieces from the mesh
	int interior = LoadTexture("KAMEN-stup.tga");
//	int interior = LoadTexture("camo.tga");
	dMatrix textureMatrix (GetIdentityMatrix());
	textureMatrix[0][0] = 1.0f / size.m_x;
	textureMatrix[1][1] = 1.0f / size.m_y;
	NewtonMesh* const convexParts = NewtonMeshVoronoiDecomposition (mesh, count, sizeof (dVector), &points[0].m_x, interior, &textureMatrix[0][0]);

#if 0
dScene xxxx(world);
dScene::dTreeNode* const modelNode = xxxx.CreateSceneNode(xxxx.GetRootNode());
dScene::dTreeNode* const meshNode = xxxx.CreateMeshNode(modelNode);
dMeshNodeInfo* const modelMesh = (dMeshNodeInfo*)xxxx.GetInfoFromNode(meshNode);
modelMesh->ReplaceMesh (convexParts);
xxxx.Serialize("xxx.xml");
#endif

	DemoEntity* const entity = new DemoEntity(NULL);
	entity->SetMatrix(*scene, dQuaternion(), dVector (0, 5, 0, 0));
	entity->InterpolateMatrix (*scene, 1.0f);
	
	
	scene->Append (entity);
	DemoMesh* const mesh1 = new DemoMesh(convexParts);
	entity->SetMesh(mesh1);
	mesh1->Release();
	

	// make sure the assets are released before leaving the function
	if (convexParts) {
		NewtonMeshDestroy (convexParts);
	}
	NewtonMeshDestroy (mesh);
	NewtonReleaseCollision(world, collision);
}
#endif


class DebriAtom
{
	public:
	DemoMesh* m_mesh;
	NewtonCollision* m_collision;
	dVector m_centerOfMass;
	dVector m_momentOfInirtia;
	dFloat m_massFraction;
};

class DebriEffect: public dList<DebriAtom> 
{
	public:

	DebriEffect(NewtonWorld* const world, NewtonMesh* const mesh, int interiorMaterial)
		:dList(), m_world (world)
	{
		// first we populate the bounding Box area with few random point to get some interior subdivisions.
		// the subdivision are local to the point placement, by placing these points visual ally with a 3d tool
		// and have precise control of how the debris are created.
		// the number of pieces is equal to the number of point inside the Mesh plus the number of point on the mesh 
		dVector size;
		dMatrix matrix(GetIdentityMatrix()); 
		NewtonMeshCalculateOOBB(mesh, &matrix[0][0], &size.m_x, &size.m_y, &size.m_z);

		// pepper the inside of the BBox box of the mesh with random points
		int count = 0;
		dVector points[NUMBER_OF_ITERNAL_PARTS];
		do {
			dFloat x = RandomVariable(size.m_x);
			dFloat y = RandomVariable(size.m_y);
			dFloat z = RandomVariable(size.m_z);
			if ((x <= size.m_x) && (x >= -size.m_x) && (y <= size.m_y) && (y >= -size.m_y) && (z <= size.m_z) && (z >= -size.m_z)){
				points[count] = dVector (x, y, z);
				count ++;
			}
		} while (count < NUMBER_OF_ITERNAL_PARTS);

		// create a texture matrix, for applying the material's UV to all internal faces
		dMatrix textureMatrix (GetIdentityMatrix());
		textureMatrix[0][0] = 1.0f / size.m_x;
		textureMatrix[1][1] = 1.0f / size.m_y;

		// now we call create we decompose the mesh into several convex pieces 
		NewtonMesh* const debriMeshPieces = NewtonMeshVoronoiDecomposition (mesh, count, sizeof (dVector), &points[0].m_x, interiorMaterial, &textureMatrix[0][0]);


		// Get the volume of the original mesh
		NewtonCollision* const collision = NewtonCreateConvexHullFromMesh (m_world, mesh, 0.0f, 0);
		dFloat volume = NewtonConvexCollisionCalculateVolume (collision);
		NewtonReleaseCollision(m_world, collision);


		// now we iterate over each pieces and for each one we create a visual entity and a rigid body
		NewtonMesh* nextDebri;
		for (NewtonMesh* debri = NewtonMeshCreateFirstSingleSegment (debriMeshPieces); debri; debri = nextDebri) {
			nextDebri = NewtonMeshCreateNextSingleSegment (debriMeshPieces, debri); 

			DebriAtom& atom = Append()->GetInfo();
			atom.m_mesh = new DemoMesh(debri);
			atom.m_collision = NewtonCreateConvexHullFromMesh (m_world, debri, 0.0f, 0);

			NewtonConvexCollisionCalculateInertialMatrix (atom.m_collision, &atom.m_momentOfInirtia[0], &atom.m_centerOfMass[0]);	
			dFloat debriVolume = NewtonConvexCollisionCalculateVolume (atom.m_collision);
			atom.m_massFraction = debriVolume / volume;

			NewtonMeshDestroy(debri);
		}

		NewtonMeshDestroy(debriMeshPieces);
	}

	DebriEffect (const DebriEffect& list)
		:dList(), m_world(list.m_world)
	{
		for (dListNode* node = list.GetFirst(); node; node = node->GetNext()) {
			DebriAtom& atom = Append(node->GetInfo())->GetInfo();
			atom.m_mesh->AddRef();
			NewtonAddCollisionReference (atom.m_collision); 
		}
	}

	~DebriEffect()
	{
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			DebriAtom& atom = node->GetInfo();
			NewtonReleaseCollision (m_world, atom.m_collision);
			atom.m_mesh->Release();
		}
	}

	NewtonWorld* m_world;
};



class SimpleShatterEffectEntity: public DemoEntity
{
	public:
	SimpleShatterEffectEntity (DemoMesh* const mesh, const DebriEffect& columnDebris)
		:DemoEntity (NULL), m_effect(columnDebris), m_myBody(NULL)
	{
		SetMesh(mesh);
	}

	~SimpleShatterEffectEntity ()
	{

	}


	void SimulationLister(DemoEntityManager* const scene, DemoEntityManager::dListNode* const mynode, dFloat timeStep)
	{

		// see if the net force on the body comes fr a high impact collision
		dVector netForce (0.0f, -m_myweight, 0.0f) ;
		for (NewtonJoint* joint = NewtonBodyGetFirstContactJoint(m_myBody); joint; joint = NewtonBodyGetNextContactJoint(m_myBody, joint)) {
			for (void* contact = NewtonContactJointGetFirstContact (joint); contact; contact = NewtonContactJointGetNextContact (joint, contact)) {
				//dVector point;
				//dVector normal;	
				dVector contactForce;
				NewtonMaterial* const material = NewtonContactGetMaterial (contact);
				//NewtonMaterialGetContactPositionAndNormal (material, &point.m_x, &normal.m_x);
				NewtonMaterialGetContactForce(material, m_myBody, &contactForce[0]);
				netForce += contactForce;
			}
		}

		dFloat mag2 = netForce % netForce ;

		// if the force is bigger than 4 Gravities, It is considered a collision force
		float maxForce = 10.0f * m_myweight;


		if (mag2 > (maxForce * maxForce)) {
			NewtonWorld* world = NewtonBodyGetWorld(m_myBody);

			dFloat Ixx; 
			dFloat Iyy; 
			dFloat Izz; 
			dFloat mass; 
			NewtonBodyGetMassMatrix(m_myBody, &mass, &Ixx, &Iyy, &Izz);

			dMatrix matrix (GetCurrentMatrix());
			dQuaternion rotation (matrix);
			for (DebriEffect::dListNode* node = m_effect.GetFirst(); node; node = node->GetNext()) {
				DebriAtom& atom = node->GetInfo();

				DemoEntity* const entity = new DemoEntity (NULL);
				entity->SetMesh (atom.m_mesh);
				entity->SetMatrix(*scene, rotation, matrix.m_posit);
				entity->InterpolateMatrix (*scene, 1.0f);
				scene->Append(entity);

				int materialId = 0;

				dFloat debriMass = mass * atom.m_massFraction;
				dFloat Ixx = debriMass * atom.m_momentOfInirtia.m_x;
				dFloat Iyy = debriMass * atom.m_momentOfInirtia.m_y;
				dFloat Izz = debriMass * atom.m_momentOfInirtia.m_z;

				//create the rigid body
				NewtonBody* const rigidBody = NewtonCreateBody (world, atom.m_collision, &matrix[0][0]);

				// set the correct center of gravity for this body
				NewtonBodySetCentreOfMass (rigidBody, &atom.m_centerOfMass[0]);

				// set the mass matrix
				NewtonBodySetMassMatrix (rigidBody, debriMass, Ixx, Iyy, Izz);

				// activate 
				//	NewtonBodyCoriolisForcesMode (blockBoxBody, 1);

				// save the pointer to the graphic object with the body.
				NewtonBodySetUserData (rigidBody, entity);

				// assign the wood id
				NewtonBodySetMaterialGroupID (rigidBody, materialId);

				//  set continue collision mode
				//	NewtonBodySetContinuousCollisionMode (rigidBody, continueCollisionMode);

				// set a destructor for this rigid body
				NewtonBodySetDestructorCallback (rigidBody, PhysicsBodyDestructor);

				// set the transform call back function
				NewtonBodySetTransformCallback (rigidBody, DemoEntity::SetTransformCallback);

				// set the force and torque call back function
				NewtonBodySetForceAndTorqueCallback (rigidBody, PhysicsApplyGravityForce);
			}

			NewtonDestroyBody(world, m_myBody);
			scene->RemoveEntity	(mynode);
		}


	};

	DebriEffect m_effect;
	NewtonBody* m_myBody;
	dFloat m_myweight; 
};


static void AddTopLayer (
	DemoEntityManager* const scene, 
	DemoMesh* const visualMesh, 
	NewtonCollision* const collision,
	const DebriEffect& shatterEffect, 
	dVector location)
{
	dQuaternion rotation;
	SimpleShatterEffectEntity* const entity = new SimpleShatterEffectEntity (visualMesh, shatterEffect);
	entity->SetMatrix(*scene, rotation, location);
	entity->InterpolateMatrix (*scene, 1.0f);
	scene->Append(entity);

	dVector origin;
	dVector inertia;
	NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

float mass = 10.0f;
int materialId = 0;

	dFloat Ixx = mass * inertia[0];
	dFloat Iyy = mass * inertia[1];
	dFloat Izz = mass * inertia[2];

	//create the rigid body
	dMatrix matrix (GetIdentityMatrix());
	matrix.m_posit = location;

	NewtonWorld* const world = scene->GetNewton();
	NewtonBody* const rigidBody = NewtonCreateBody (world, collision, &matrix[0][0]);


	entity->m_myBody = rigidBody;
	entity->m_myweight = dAbs (mass * DEMO_GRAVITY);

	// set the correct center of gravity for this body
	NewtonBodySetCentreOfMass (rigidBody, &origin[0]);

	// set the mass matrix
	NewtonBodySetMassMatrix (rigidBody, mass, Ixx, Iyy, Izz);

	// activate 
	//	NewtonBodyCoriolisForcesMode (blockBoxBody, 1);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (rigidBody, entity);

	// assign the wood id
	NewtonBodySetMaterialGroupID (rigidBody, materialId);

	//  set continue collision mode
	//	NewtonBodySetContinuousCollisionMode (rigidBody, continueCollisionMode);

	// set a destructor for this rigid body
	NewtonBodySetDestructorCallback (rigidBody, PhysicsBodyDestructor);

	// set the transform call back function
	NewtonBodySetTransformCallback (rigidBody, DemoEntity::SetTransformCallback);

	// set the force and torque call back function
	NewtonBodySetForceAndTorqueCallback (rigidBody, PhysicsApplyGravityForce);

//	return rigidBody;
}


static void AddColumnsLayer (DemoEntityManager* const scene, DemoMesh* const visualMesh, NewtonCollision* const collision, const DebriEffect& shatterEffect, dVector location, float separation)
{
	//create and array of entities all reusing the same effect
	for (int i = 0; i < 2; i ++) {
		for (int j = 0; j < 2; j ++) {
			float x = (float (i) - 0.5f) * separation + location.m_x;
			float z = (float (j) - 0.5f) * separation + location.m_z;
			dQuaternion rotation;
			dVector position (x, location.m_y, z, 1.0f);
			AddTopLayer (scene, visualMesh, collision, shatterEffect, position);
		}
	}
}



static void Stonehenge (DemoEntityManager* const scene, dVector location, int levels, int xCount, int zCount, float separation)
{
	NewtonWorld* const world = scene->GetNewton();


	// Create the vertical column effect
	dVector size (1.0f, 4.0f, 1.0f);
//	NewtonCollision* const verticalColumnCollision = CreateConvexCollision (world, GetIdentityMatrix(), size, _RANDOM_CONVEX_HULL_PRIMITIVE, 0);
	NewtonCollision* const verticalColumnCollision = CreateConvexCollision (world, GetIdentityMatrix(), size, _BOX_PRIMITIVE, 0);

	// create a newton mesh from the collision primitive
	NewtonMesh* const verticalColumnNewtonMesh = NewtonMeshCreateFromCollision(verticalColumnCollision);

	// apply a material map
	int columnMaterial = LoadTexture("KAMEN-stup.tga");
	NewtonMeshApplyBoxMapping(verticalColumnNewtonMesh, columnMaterial, columnMaterial, columnMaterial);

	// shatter this mesh into pieces
	DebriEffect columnDebris (world, verticalColumnNewtonMesh, columnMaterial);

	// create the visual Mesh
	DemoMesh* const visualColumnMesh = new DemoMesh(verticalColumnNewtonMesh);

	// calculate the floor position for this objects
	dVector meshBounds;
	dMatrix matrix (GetIdentityMatrix());
	NewtonMeshCalculateOOBB(verticalColumnNewtonMesh, &matrix[0][0], &meshBounds.m_x, &meshBounds.m_y, &meshBounds.m_z);


	// now calculate the top mesh
	float topSize = 10.0f;
//	size = dVector (topSize, 0.2f, topSize, 0.0f);
	size = dVector (topSize, 0.5f, topSize, 0.0f);
	NewtonCollision* const topSlabCollision = CreateConvexCollision (world, GetIdentityMatrix(), size, _BOX_PRIMITIVE, 0);
	NewtonMesh* const topNewtonMesh = NewtonMeshCreateFromCollision(topSlabCollision);
	int topMaterial = LoadTexture("reljef.tga");
	NewtonMeshApplyBoxMapping(topNewtonMesh, columnMaterial, topMaterial, columnMaterial);

	// create a newton mesh from the collision primitive
	DebriEffect topDebris (world, topNewtonMesh, columnMaterial);

	// create the visual top mesh
	DemoMesh* const visualTopMesh = new DemoMesh(topNewtonMesh);


	//create and array of entities all reusing the same effect
	for (int i = 0; i < zCount; i ++) {
		for (int j = 0; j < xCount; j ++) {
			float x = (float (i) - 0.5f) * separation + location.m_x;
			float z = (float (j) - 0.5f) * separation + location.m_z;

			for (int i = 0; i < levels; i ++) {
				// add a column layer
//				dVector posit (location.m_x, FindFloor (world, location.m_x, location.m_z) + meshBounds.m_x, location.m_z);
				dVector posit (x, FindFloor (world, x, z) + meshBounds.m_x, z);
				AddColumnsLayer (scene, visualColumnMesh, verticalColumnCollision, columnDebris, posit, topSize - 1.0f);

				// add the top
				float xColum = (-0.5f) * (topSize - 1.0f) + x;
				float zColum = (-0.5f) * (topSize - 1.0f) + z;

//				posit = dVector (location.m_x, FindFloor (world, x, z) + size.m_y * 0.5f, location.m_z);
				posit = dVector (x, FindFloor (world, xColum, zColum) + size.m_y * 0.5f, z);
				AddTopLayer (scene, visualTopMesh, topSlabCollision, topDebris, posit);
			}
		}
	}

	// make sure we release reference to all assets

	visualTopMesh->Release();
	NewtonMeshDestroy (topNewtonMesh);
	NewtonReleaseCollision(world, topSlabCollision);


	visualColumnMesh->Release();
	NewtonMeshDestroy (verticalColumnNewtonMesh);
	NewtonReleaseCollision(world, verticalColumnCollision);
}



void SimpleConvexShatter (DemoEntityManager* const scene)
{
	// suspend simulation before making changes to the physics world
	scene->StopsExecution ();

	// load the skybox
	scene->Append(new SkyBox());

	// load the scene from and alchemedia file format
	CreateLevelMesh (scene, "flatPlane.xml", false);
	//CreateLevelMesh (scene, "sponza.xml", false);

	// create a shattered mesh array
	Stonehenge (scene, dVector (0.0f, 0.0f, 0.0f, 0.0f), 5, 2, 2, 30.0f);
// CreateSimpleVoronoiShatter (scene);


	// place camera into position
	dQuaternion rot;
	dVector origin (-40.0f, 10.0f, 0.0f, 0.0f);
	scene->SetCameraMatrix(rot, origin);

	// resume the simulation
	scene->ContinueExecution();
}


