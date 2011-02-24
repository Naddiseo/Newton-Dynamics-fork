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



//#define USE_TEST_SERIALIZATION

#ifdef USE_TEST_SERIALIZATION

static const char* MAGIC_NUMBER = "serialize data";
static void SerializeFile (void* serializeHandle, const void* buffer, int size)
{
	fwrite (buffer, size, 1, (FILE*) serializeHandle);
}

static void DeSerializeFile (void* serializeHandle, void* buffer, int size)
{
	fread (buffer, size, 1, (FILE*) serializeHandle);
}

#endif


#ifdef USE_TEST_ALL_FACE_USER_RAYCAST_CALLBACK
static dFloat AllRayHitCallback (dFloat interseption, dFloat* normal, int faceId, void* userData)
{
	return interseption;
}
#endif


/*
void ScenePrimitive::AddCollisionTree(NewtonCollision* scene, const NewtonWorld* nWorld, const char* name, int optimized)
{
	NewtonCollision* collision;
	NewtonSceneProxy* sceneProxy;
	
	// open the level data
	char fullPathName[2048];
	GetWorkingFileName (name, fullPathName);

	OGLLoaderContext context;
	dMatrix matrix (dYawMatrix (-3.14159265f * 0.5f));

_ASSERTE (0);
//	LoadCollada(fullPathName, context, matrix, 1.0f);

	// create the collision tree geometry
	collision = NewtonCreateTreeCollision(nWorld, 0);

	// prepare to create collision geometry
	NewtonTreeCollisionBeginBuild(collision);

	dMatrix pallete[64];
	UpdateMatrixPalette (GetIdentityMatrix(), pallete, sizeof (m_matrixPalete) / sizeof (dMatrix));

	// iterate the entire geometry an build the collision
//	for (ModelComponentList<dList<dMesh*> >::dListNode* list = m_meshList.GetFirst(); list; list = list->GetNext()) {
	for (dList<dMeshInstance>::dListNode* node = m_meshList.GetFirst(); node; node = node->GetNext()) { 
		dFloat* vertex;
	
		OGLMesh* mesh = (OGLMesh*)node->GetInfo().m_mesh;

		vertex = mesh->m_vertex;
		const dMatrix& globalMatrix = pallete[mesh->m_boneID];
		for (dMesh::dListNode* nodes = mesh->GetFirst(); nodes; nodes = nodes->GetNext()) {
			dSubMesh& segment = nodes->GetInfo();
			for (int i = 0; i < segment.m_indexCount; i += 3) {
				int index;	
				int matID;
				dVector face[3];

				index = segment.m_indexes[i + 0] * 3;
				face[0] = dVector (vertex[index + 0], vertex[index + 1], vertex[index + 2]);

				index = segment.m_indexes[i + 1] * 3;
				face[1] = dVector (vertex[index + 0], vertex[index + 1], vertex[index + 2]);

				index = segment.m_indexes[i + 2] * 3;
				face[2] = dVector (vertex[index + 0], vertex[index + 1], vertex[index + 2]);

				globalMatrix.TransformTriplex (face, sizeof (dVector), face, sizeof (dVector), 3);

				// stress test the collision builder
//				matID = matID == 2 ? 1 : 2 ;
				matID = 0;
				NewtonTreeCollisionAddFace(collision, 3, &face[0].m_x, sizeof (dVector), matID);
			}
		}
	}
	NewtonTreeCollisionEndBuild(collision, optimized);

	// create a Scene proxy to contain this mesh
	sceneProxy = NewtonSceneCollisionCreateProxy (scene, collision);

	// release the collision tree (this way the application does not have to do book keeping of Newton objects
	NewtonReleaseCollision (nWorld, collision);

	// set proxy relative matrix 
	dMatrix proxyMatrix (GetIdentityMatrix());
	NewtonSceneProxySetMatrix (sceneProxy, &proxyMatrix[0][0]);

	// save the pointer to the graphic object with the body.
//	NewtonBodySetUserData (m_level, this);


#ifdef USE_TEST_ALL_FACE_USER_RAYCAST_CALLBACK
	// set a ray cast callback for all face ray cast 
	NewtonTreeCollisionSetUserRayCastCallback (collision, AllRayHitCallback);
	dVector p0 (0,  100, 0, 0);
	dVector p1 (0, -100, 0, 0);
	dVector normal;
	int id;
	dFloat parameter;
	parameter = NewtonCollisionRayCast (collision, &p0[0], &p1[0], &normal[0], &id);
#endif

}

void ScenePrimitive::DebugCallback (const NewtonBody* bodyWithTreeCollision, const NewtonBody* body, int faceID, int vertexCount, const dFloat* vertex, int vertexstrideInBytes)
{
	// the application can use this function for debugging purpose by writing the 
	// face to a global variable then display which face of the mesh are been used for collision.
	// ,,,,,,,,,,,
	// ..........

	// the application can use this function also to modify the collision geometry by changing the face ID
	// this could be uses full to make surface change for example from dry to wet, or breaking glass
	// for this the application should used the functions:
	// int id = NewtonTreeCollisionGetFaceAtribute (treeCollision, indexArray); 
	// NewtonTreeCollisionSetFaceAtribute (treeCollision, indexArray, new id);

	if (DebugDisplayOn()) {
		dVector face[64];
		int stride = vertexstrideInBytes / sizeof (dFloat);
		for (int j = 0; j < vertexCount; j ++) {
			face [j] = dVector (vertex[j * stride + 0], vertex[j * stride + 1] , vertex[j * stride + 2]);
		}
		DebugDrawPolygon (vertexCount, face);
	}
}
*/



class ComplexScene: public DemoEntity 
{
	public:
	ComplexScene (NewtonCollision* const sceneCollision)
		:DemoEntity (NULL)
		,m_sceneCollision (sceneCollision)
	{
		
	}

	~ComplexScene ()
	{
		for (NewtonSceneProxy* proxy = NewtonSceneGetFirstProxy(m_sceneCollision); proxy; proxy = NewtonSceneGetNextProxy(m_sceneCollision, proxy)) {
			DemoMesh* const mesh = (DemoMesh*) NewtonSceneGetProxyUserData (proxy);
			mesh->Release();
		}
	}

	void Render(dFloat timeStep) const
	{
		// save the model matrix before changing it Matrix
		glPushMatrix();

		// Set The matrix for this entity Node
		glMultMatrix(&m_matrix[0][0]);

		// Render mesh if there is one 
		for (NewtonSceneProxy* proxy = NewtonSceneGetFirstProxy(m_sceneCollision); proxy; proxy = NewtonSceneGetNextProxy(m_sceneCollision, proxy)) {
			dMatrix matrix;
			//const VisualProxy& proxy = node->GetInfo();
			
			DemoMesh* const mesh = (DemoMesh*) NewtonSceneGetProxyUserData (proxy);

			glPushMatrix();
			NewtonSceneProxyGetMatrix(proxy, &matrix[0][0]);
			glMultMatrix(&matrix[0][0]);
			mesh->Render();

			glPopMatrix();
		}


		// restore the matrix before leaving
		glPopMatrix();
	}

	void AddCollisionTreeMesh (DemoEntityManager* const scene)
	{
		// open the level data
		char fullPathName[2048];
		GetWorkingFileName ("playground.xml", fullPathName);

		scene->LoadScene (fullPathName);

		// find the visual mesh and make a collision tree
		NewtonWorld* const world = scene->GetNewton();
		DemoEntity* const ent = scene->GetLast()->GetInfo();
		DemoMesh* const mesh = ent->GetMesh();

		NewtonCollision* const tree = NewtonCreateTreeCollision(world, 0);
		// prepare to create collision geometry
		NewtonTreeCollisionBeginBuild(tree);

		dFloat* const vertex = mesh->m_vertex;
		for (DemoMesh::dListNode* node = mesh->GetFirst(); node; node = node->GetNext()){
			DemoSubMesh* const subMesh = &node->GetInfo();
			unsigned int* const indices = subMesh->m_indexes;
			int trianglesCount = subMesh->m_indexCount;
			for (int i = 0; i < trianglesCount; i += 3) {

				dVector face[3];
				int index = indices[i + 0] * 3;
				face[0] = dVector (vertex[index + 0], vertex[index + 1], vertex[index + 2]);

				index = indices[i + 1] * 3;
				face[1] = dVector (vertex[index + 0], vertex[index + 1], vertex[index + 2]);

				index = indices[i + 2] * 3;
				face[2] = dVector (vertex[index + 0], vertex[index + 1], vertex[index + 2]);

				int matID = 0;
				//matID = matID == 2 ? 1 : 2 ;
				NewtonTreeCollisionAddFace(tree, 3, &face[0].m_x, sizeof (dVector), matID);
			}
		}
		NewtonTreeCollisionEndBuild (tree, 0);

		dMatrix matrix (ent->GetCurrentMatrix());

		// create a Scene proxy to contain this mesh
		NewtonSceneProxy* const sceneProxy = NewtonSceneCollisionCreateProxy (m_sceneCollision, tree, &matrix[0][0]);

		// release the collision tree (this way the application does not have to do book keeping of Newton objects
		NewtonReleaseCollision (world, tree);

		NewtonSceneSetProxyUserData (sceneProxy, mesh);
		mesh->AddRef();

		scene->RemoveEntity (ent);

	#ifdef USE_TEST_ALL_FACE_USER_RAYCAST_CALLBACK
		// set a ray cast callback for all face ray cast 
		NewtonTreeCollisionSetUserRayCastCallback (collision, AllRayHitCallback);
		dVector p0 (0,  100, 0, 0);
		dVector p1 (0, -100, 0, 0);
		dVector normal;
		int id;
		dFloat parameter;
		parameter = NewtonCollisionRayCast (collision, &p0[0], &p1[0], &normal[0], &id);
	#endif

	}

	void AddPrimitives(DemoEntityManager* const scene)
	{
		dMatrix rotMat (dRollMatrix (0.5f * 3.141592f));

		NewtonWorld* const world = scene->GetNewton();
		NewtonCollision* collision2 = NewtonCreateCylinder (world, 0.5f, 0.2f, 0, &rotMat[0][0]);
		DemoMesh* const geometry2 = new DemoMesh("cylinder_1", collision2, "smilli.tga", "smilli.tga", "smilli.tga");

		NewtonCollision* collision1 = NewtonCreateBox (world, 0.75f, 0.2f, 0.75f, 0, NULL);
		DemoMesh* const geometry1 = new DemoMesh("box_1", collision1, "frowny.tga", "frowny.tga", "frowny.tga");

		dMatrix matrix (GetIdentityMatrix());
		for (int y = 0; y < 10; y ++) {
			for (int x = 0; x < 10; x ++) {
				for (int z = 0; z < 10; z ++) {
					DemoMesh* mesh;
					NewtonCollision* collision;
					
					matrix.m_posit.m_y = 4.0f + y * 1.0f ; 
					matrix.m_posit.m_x = x * 1.5f - 8.0f; 
					matrix.m_posit.m_z = z * 1.5f - 6.0f;  
					
					if (rand () & 0x010) {
					//if (0) {
						mesh = geometry1;
						collision = collision1;
					} else {
						mesh = geometry2;
						collision = collision2;
					}

					NewtonSceneProxy* const sceneProxy = NewtonSceneCollisionCreateProxy (m_sceneCollision, collision, &matrix[0][0]);
					NewtonSceneSetProxyUserData (sceneProxy, mesh);
					mesh->AddRef();
				}
			}
		}

		geometry1->Release();
		geometry2->Release();
		NewtonReleaseCollision (world, collision1);
		NewtonReleaseCollision (world, collision2);
	}

	NewtonCollision* m_sceneCollision;

};








void SceneCollision (DemoEntityManager* const scene)
{
	// suspend simulation before making changes to the physics world
	scene->StopsExecution ();


	NewtonWorld* world = scene->GetNewton();

	// add the Sky
	scene->Append(new SkyBox());


	// create a body and with a scene collision
	NewtonCollision* sceneCollision = NewtonCreateSceneCollision (scene->GetNewton(), 0);

	// create a visual scene empty mesh
	ComplexScene* const visualMesh = new ComplexScene(sceneCollision);
	scene->Append (visualMesh);


	// add a collision tree to the scene
	visualMesh->AddCollisionTreeMesh(scene);

	// add some shapes
	visualMesh->AddPrimitives(scene);

	// this is optional, finish the scene construction, optimize the collision scene
	NewtonSceneCollisionOptimize (sceneCollision);


	// set the world size base on the scene size
	dVector boxP0; 
	dVector boxP1; 
	// get the position of the aabb of this geometry
	dMatrix matrix (GetIdentityMatrix());
	NewtonCollisionCalculateAABB (sceneCollision, &matrix[0][0], &boxP0.m_x, &boxP1.m_x); 

	// add some extra padding the world size
	boxP0.m_x -=  50.0f;
	boxP0.m_y -=  50.0f;
	boxP0.m_z -=  50.0f;
	boxP1.m_x +=  50.0f;
	boxP1.m_y += 500.0f;
	boxP1.m_z +=  50.0f;

	// set the world size
	NewtonSetWorldSize (world, &boxP0.m_x, &boxP1.m_x); 

	// set the application level callback
#ifdef USE_STATIC_MESHES_DEBUG_COLLISION
	NewtonStaticCollisionSetDebugCallback (sceneCollision, DebugCallback);
#endif


#ifdef USE_TEST_SERIALIZATION
	{
		// enumerate the proxies and save the userdata before serializing
		int index = 0;
		dTree<DemoMesh*, int> saveData;
		for (NewtonSceneProxy* proxy = NewtonSceneGetFirstProxy(sceneCollision); proxy; proxy = NewtonSceneGetNextProxy(sceneCollision, proxy)) {
			saveData.Insert((DemoMesh*)NewtonSceneGetProxyUserData(proxy), index);
			NewtonSceneSetProxyUserData(proxy, (void*)index);
			index ++;
		}
		

		// save the collision file
		char fullPathName[256];
		GetWorkingFileName ("collisiontest.bin", fullPathName);

		FILE* file = fopen (fullPathName, "wb");
		SerializeFile (file, MAGIC_NUMBER, int (strlen (MAGIC_NUMBER)) + 1);
		NewtonCollisionSerialize (world, sceneCollision, SerializeFile, file);
		fclose (file);

		// load the collision file
		NewtonReleaseCollision (world, sceneCollision);

		file = fopen (fullPathName, "rb");
		char magicNumber[256]; 
		DeSerializeFile (file, magicNumber, int (strlen (MAGIC_NUMBER)) + 1);
		sceneCollision = NewtonCreateCollisionFromSerialization (world, DeSerializeFile, file);
		fclose (file);

		// restore the user info form teh save data and set userdata on each proxy
		for (NewtonSceneProxy* proxy = NewtonSceneGetFirstProxy(sceneCollision); proxy; proxy = NewtonSceneGetNextProxy(sceneCollision, proxy)) {
			int index = int (NewtonSceneGetProxyUserData(proxy));
			
			dTree<DemoMesh*, int>::dTreeNode* const node = saveData.Find(index);
			NewtonSceneSetProxyUserData(proxy, node->GetInfo());
		}

		// see if the data is valid
		//NewtonCollisionInfoRecord collisionInfo;
		//NewtonCollisionGetInfo (sceneCollision, &collisionInfo);
		//NewtonCollisionGetInfo (sceneCollision, &collisionInfo);
	}
#endif


	// create the level body and add it to the world
	NewtonBody* const level = NewtonCreateBody (world, sceneCollision, &matrix[0][0]);

	// set the reference to the visual
	NewtonBodySetUserData(level, visualMesh);

	// do not forget to release the collision
	NewtonReleaseCollision (world, sceneCollision);


	// add few objects



	dMatrix camMatrix (dRollMatrix(-20.0f * 3.1416f /180.0f) * dYawMatrix(-45.0f * 3.1416f /180.0f));
	dQuaternion rot (camMatrix);
	dVector origin (-15.0f, 15.0f, -15.0f, 0.0f);
	scene->GetCamera()->SetMatrix (*scene, rot, origin);
	scene->GetCamera()->SetMatrix (*scene, rot, origin );

	// resume the simulation
	scene->ContinueExecution();
}