/* Copyright (c) <2003-2011> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "Common.h"
#include "Export.h"
#include "options.h"




#define D_MAX_UV_CHANNELS 4

/*
class MaxExportMesh: public dMesh
{
	public: 
	MaxExportMesh(int vertexCount)
		:dMesh ("")
	{
		m_maxVertexIndex = new int [vertexCount];
	}

	~MaxExportMesh()
	{
		delete[] m_maxVertexIndex;
	}

	int* m_maxVertexIndex;
};


class KeyFrame
{
	public:
	dVector m_posit;
	dQuaternion m_rotation;
	int m_frame;
};

class AnimationNode: public dList <KeyFrame>
{
public:
	char m_nodeName[32]; 

};

class AnimationClip: public dList<AnimationNode>
{
	public:
	void Load (INode* node, dAnimationClip* clip)
	{
		int index;
		dList<INode*> list;
		INode* stack[1024];

		stack[0] = node;
		index = 1;
		while (index ) {
			int childrenCount;
			index --;
			node = stack[index];

			LoadNodeAnimation (node);

			childrenCount = node->NumberOfChildren();
			for (int i = 0; i < childrenCount; i ++) {
				stack[index] = node->GetChildNode(childrenCount - i - 1);
				index	++;
				_ASSERTE (index * sizeof (INode*) < sizeof (stack));	
			}
		}

		dMatrix rotation (dRollMatrix (-3.14159265f * 0.5f) * dPitchMatrix(-3.14159265f * 0.5f));
		dMatrix invRotation (rotation.Inverse());
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			for (AnimationNode::dListNode* keyFrames = node->GetInfo().GetFirst(); keyFrames; keyFrames = keyFrames->GetNext()) {
				KeyFrame& keyFrame = keyFrames->GetInfo();

				dMatrix matrix (keyFrame.m_rotation, keyFrame.m_posit);
				matrix = invRotation * matrix * rotation;

				keyFrame.m_rotation = dQuaternion (matrix);
				keyFrame.m_posit = matrix.m_posit;
			}
		}

		clip->SetFramesCount (((m_end - m_start) / m_fps) + 1);
		for (dListNode* node = GetFirst(); node; node = node->GetNext()) {
			dKeyFrames& keyFrames = clip->Append()->GetInfo();
			AnimationNode& track = node->GetInfo();

			strcpy (keyFrames.m_bindName, track.m_nodeName);
			keyFrames.AllocaFrames (track.GetCount());

			int index;
			index = 0;
			for (AnimationNode::dListNode* trackKeyFrames = node->GetInfo().GetFirst(); trackKeyFrames; trackKeyFrames = trackKeyFrames->GetNext()) {
				KeyFrame& trackKey = trackKeyFrames->GetInfo();;

				keyFrames.m_keys[index] = trackKey.m_frame / m_fps;
				keyFrames.m_posit[index] = trackKey.m_posit;
				keyFrames.m_rotation[index] = trackKey.m_rotation;
				index ++;
			}
		}
	}


	void GetMatrixAnimationMatrix (INode* node, int frame, dQuaternion& rotation, dVector& position) const
	{
		dMatrix matrix (GetMatrixFromMaxMatrix (node->GetNodeTM (frame)));
		dMatrix parent (GetMatrixFromMaxMatrix (node->GetParentTM(frame)));
		matrix = matrix * parent.Inverse();
		rotation = dQuaternion (matrix);
		position = matrix.m_posit;
	}


	void LoadNodeAnimation (INode* node)
	{
		bool animated;
		dVector firstPos;
		dQuaternion firstRot;

		animated = false;
		for (int t = m_start; t <= m_end; t += m_fps) {
			dVector pos;
			dQuaternion rot;

			GetMatrixAnimationMatrix (node, t, rot, pos);
			if (t != m_start) {
				dQuaternion errorRot (rot - firstRot);
				dVector errorPosit (pos - firstPos);
				if (((errorPosit % errorPosit) > 1.0e-6f) || (errorRot.DotProduct(errorRot) > 1.0e-6f)) {
					animated = true;
					break;
				}
			} else {
				firstRot = rot;
				firstPos = pos;
			}
		}

		if (animated) {
			AnimationNode* keyFrames;
			keyFrames = &Append()->GetInfo();

			GetNodeName (node, keyFrames->m_nodeName);

			GetMatrixAnimationMatrix (node, m_start, firstRot, firstPos);
			KeyFrame& keyFrame0 = keyFrames->Append()->GetInfo ();
			keyFrame0.m_frame = m_start;
			keyFrame0.m_posit = firstPos;
			keyFrame0.m_rotation = firstRot;

			for (int t = m_start; t < m_end; t += m_fps) {
				dVector pos;
				dQuaternion rot;
				GetMatrixAnimationMatrix (node, t, rot, pos);

				dQuaternion errorRot (rot - firstRot);
				dVector errorPosit (pos - firstPos);
				if (((errorPosit % errorPosit) > 1.0e-6f) || (errorRot.DotProduct(errorRot) > 1.0e-6f)) {
					firstRot = rot;
					firstPos = pos;

					KeyFrame& keyFrame = keyFrames->Append()->GetInfo ();
					keyFrame.m_frame = t;
					keyFrame.m_posit = firstPos;
					keyFrame.m_rotation = firstRot;
				}
			}

			GetMatrixAnimationMatrix (node, m_end, firstRot, firstPos);
			KeyFrame& keyFrame1 = keyFrames->Append()->GetInfo ();
			keyFrame1.m_frame = m_end;
			keyFrame1.m_posit = firstPos;
			keyFrame1.m_rotation = firstRot;
		}
	}

	int m_start;
	int m_end;
	int m_fps;
};
*/







/*
void Export::FlattenMesh (dModel& model)
{
	int bonesCount;
	dMatrix matrix (GetIdentityMatrix());
	dMatrix pallette[256];
	
	bonesCount = model.GetBoneCount();
	_ASSERTE (bonesCount < sizeof (pallette) / sizeof (pallette[0]));

	model.UpdateMatrixPalette (GetIdentityMatrix(), pallette, bonesCount);
//	dMesh* mesh;
//	mesh = new dMesh();
	int vertexCount = 0;
	for (ModelComponentList<dList<dMeshInstance> >::dListNode* list = model.m_meshList.GetFirst(); list; list = list->GetNext()) {
		for (dList<dMeshInstance>::dListNode* node = list->GetInfo().m_data.GetFirst(); node; node = node->GetNext()) { 
			dMesh* mesh = node->GetInfo();
			vertexCount +=  mesh->m_vertexCount;
		}
	}
//	mesh->AllocVertexData(vertexCount);
}
*/




void Export::GetNodeList (Tab<INode *>& nodeTab)
{
	int stackIndex;
	INode* stack[1024];

	stackIndex = 1;
	stack[0] = m_ip->GetRootNode();

	while (stackIndex) {
		stackIndex --;
		INode* node = stack[stackIndex];
		nodeTab.Append(1, &node); 

		for (int i = 0; i < node->NumberOfChildren(); i ++) {
			stack[stackIndex] = node->GetChildNode(i);
			stackIndex ++;
			_ASSERTE (stackIndex * sizeof (INode*) < sizeof (stack));	
		}
	}
}


//void Export::LoadSkeletonAndGeomtry (INode* node, dModel& model)
void Export::LoadSkeletonAndGeomtry (INode* const node, dScene& scene)
{
	INode* stack[1024];
	dScene::dTreeNode* parentNode[1024];

	stack[0] = node;
	dScene::dTreeNode* root = scene.GetRootNode();

	int stackIndex = 0;
	for (int i = 0; i < node->NumberOfChildren(); i ++) {
		parentNode[stackIndex] = root;
		stack[stackIndex] = node->GetChildNode(i);
		stackIndex ++;
	}

	NodeMap nodeMap;
	while (stackIndex) {
		char nameTmp[256];

		stackIndex --;
		INode* const node = stack[stackIndex];
		dScene::dTreeNode* parent = parentNode[stackIndex];

		GetNodeName (node, nameTmp);
		TSTR name = nameTmp;

		dScene::dTreeNode* sceneNode = NULL;
		dSceneNodeInfo* info = NULL;
		if (node->GetBoneNodeOnOff()) {
			sceneNode = scene.CreateBoneNode(parent);
			info = (dBoneNodeInfo*) scene.GetInfoFromNode (sceneNode);
		} else {
			sceneNode = scene.CreateSceneNode(parent);
			info = (dSceneNodeInfo*) scene.GetInfoFromNode (sceneNode);
		}

		nodeMap.Insert(sceneNode, node);
		info->SetName(name);
		//bone->SetType(node->GetBoneNodeOnOff() ? dBone::m_bone : dBone::m_sceneNode);


		// Get Transformation matrix at frame 0
		dMatrix matrix (GetMatrixFromMaxMatrix (node->GetNodeTM (0)));
		//dMatrix rootMatrix (parent->CalcGlobalMatrix ());
		//matrix = matrix * rootMatrix.Inverse();
		//matrix.m_front = matrix.m_front.Scale (1.0f / dSqrt (matrix.m_front % matrix.m_front));
		//matrix.m_up = matrix.m_right * matrix.m_front;
		//matrix.m_up = matrix.m_up.Scale (1.0f / dSqrt (matrix.m_up % matrix.m_up));
		//matrix.m_right = matrix.m_front * matrix.m_up;
		//bone->SetMatrix (matrix);
		info->SetTransform(matrix);

//		bone->m_boneID = boneIndex;
//		boneIndex ++;
		for (int i = 0; i < node->NumberOfChildren(); i ++) {
			parentNode[stackIndex] = sceneNode;
			stack[stackIndex] = node->GetChildNode(i);
			stackIndex ++;
			_ASSERTE (stackIndex * sizeof (INode*) < sizeof (stack));	
		}
	}

	LoadGeometries (m_ip->GetRootNode(), scene, nodeMap);
}


void Export::LoadGeometries (INode* const node, dScene& scene, NodeMap& nodeMap)
{
	INode* stack[4096];
	dTree<dScene::dTreeNode*, Object*> instanceFilter;

	int stackIndex = 0;
	for (int i = 0; i < node->NumberOfChildren(); i ++) {
		stack[stackIndex] = node->GetChildNode(i);
		stackIndex ++;
	}

	int materialID = 0;
	ImageCache imageCache;
	while (stackIndex) {

		stackIndex --;
		INode* const node = stack[stackIndex];

		ObjectState os (node->EvalWorldState(0)); 

		// The obj member of ObjectState is the actual object we will export.
		if (os.obj) {

			dTree<dScene::dTreeNode*, Object*>::dTreeNode* meshNode;
			meshNode = instanceFilter.Find(os.obj);
			
			if (!meshNode) {

				// We look at the super class ID to determine the type of the object.
				switch(os.obj->SuperClassID()) 
				{
					case GEOMOBJECT_CLASS_ID: 
					{
						if (!node->GetBoneNodeOnOff()) {

							dScene::dTreeNode* meshInstance = LoadTriangleObject (node, &os, scene, nodeMap, imageCache, materialID);
							if (meshInstance) {
								meshNode = instanceFilter.Insert(meshInstance, os.obj);

								//dMesh* mesh;
								//dBone* bone;
								//char name[256];
								//mesh = meshInstance->m_mesh;
								//GetNodeName (node, name);
								//bone = model.FindBone(name);
								//_ASSERTE (bone);
								//mesh->m_hasBone = 1;
								//mesh->m_boneID = bone->m_boneID;
								//meshInstance->m_boneID = bone->m_boneID;
							}
						}
						break;
					}

					case HELPER_CLASS_ID:
					{
	//					LoadHelperObject (node, &os, model);
						break;
					}

					case CAMERA_CLASS_ID:
						_ASSERTE (0);
		//				if (GetIncludeObjCamera()) ExportCameraObject(node, indentLevel); 
						break;
					case LIGHT_CLASS_ID:
						_ASSERTE (0);
		//				if (GetIncludeObjLight()) ExportLightObject(node, indentLevel); 
						break;
					case SHAPE_CLASS_ID:
		//				_ASSERTE (0);
		//				if (GetIncludeObjShape()) ExportShapeObject(node, indentLevel); 
						break;
					default:
						_ASSERTE (0);
				}
			} else {
				_ASSERTE (nodeMap.Find (node));
				dScene::dTreeNode* parentNode = nodeMap.Find (node)->GetInfo();
				dScene::dTreeNode* meshInstance = meshNode->GetInfo();
				scene.AddReference(parentNode, meshInstance);
			}
		}

		for (int i = 0; i < node->NumberOfChildren(); i ++) {
			stack[stackIndex] = node->GetChildNode(i);
			stackIndex ++;
			_ASSERTE (stackIndex * sizeof (INode*) < sizeof (stack));	
		}
	}
}


#if 1

dVector Export::CalcVertexNormal (MNMesh& mesh, int faceNo, int faceIndex) const
{
	//I can no figure out how to get the vertex normal for a NNMesn
	return dVector (0.0f, 0.0f, 0.0f, 0.0f);
/*
//	int i;
//	int vertexIndex;
//	int numNormals;
//	Face* face;
//	RVertex* rv;
//	DWORD smGroup;
//	Point3 normal;
	dVector vertexNormal (0.0f, 0.0f, 0.0f, 0.0f);

	MNFace* const face = mesh.F(faceNo);

	int vertexIndex = face->vtx[faceIndex];

	DWORD smGroup = face->smGroup;
//	RVertex* const rv = mesh.rVerts;
//	_ASSERTE (rv);

//	int numNormals = rv->rFlags & NORCT_MASK;

	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) {
		_ASSERTE (0);
		normal = rv->rn.getNormal();
		vertexNormal.m_x = normal.x;
		vertexNormal.m_y = normal.y;
		vertexNormal.m_z = normal.z;

	} else if (numNormals && smGroup) {
		// If normal is not specified it's only available if the face belongs
		// to a smoothing group

		if (numNormals == 1) {
			// If there is only one vertex is found in the rn member.
			normal = rv->rn.getNormal();
			vertexNormal.m_x = normal.x;
			vertexNormal.m_y = normal.y;
			vertexNormal.m_z = normal.z;

		} else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (i = 0; i < numNormals; i++) {
				if (rv->ern[i].getSmGroup() & smGroup) {
					normal = rv->ern[i].getNormal();
					vertexNormal.m_x += normal.x;
					vertexNormal.m_y += normal.y;
					vertexNormal.m_z += normal.z;
				}
			}
		}
	} else {
		// Get the normal from the Face if no smoothing groups are there
		normal = max5Mesh.getFaceNormal(faceNo);
		vertexNormal.m_x = normal.x;
		vertexNormal.m_y = normal.y;
		vertexNormal.m_z = normal.z;
	}

	return vertexNormal;
*/
}



PolyObject* Export::GetPolyObject (ObjectState* const os, int& deleteIt)
{
	PolyObject* poly = NULL;
	deleteIt = FALSE;
	Object* const obj = os->obj;
	if (obj->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0))) { 
		poly = (PolyObject *) obj->ConvertToType(0, Class_ID(POLYOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != poly) {
			deleteIt = TRUE;
		}
	}
	return poly;
}


dScene::dTreeNode* Export::LoadTriangleObject(INode* const node, ObjectState* const os, dScene& scene, NodeMap& nodeMap, ImageCache& imageCache, int& materialID)
{
	// Targets are actually geomobjects, but we will export them
	// from the camera and light objects, so we skip them here.
	if (os->obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0)) {
		return NULL;
	}

	BOOL needDel = FALSE;
	PolyObject* const poly = GetPolyObject (os, needDel); 
	if (!poly) {
		return NULL;
	}

	MNMesh& maxMesh = poly->GetMesh();

	int facesCount = maxMesh.FNum();
	int vertexCount = maxMesh.VNum();
	if (!facesCount) {
		if (needDel) {
			delete poly;
		}
		return NULL;
	}

	_ASSERTE (nodeMap.Find (node));
	dScene::dTreeNode* const parentNode = nodeMap.Find (node)->GetInfo();
	dSceneNodeInfo* const parent = (dSceneNodeInfo*) scene.GetInfoFromNode(parentNode);


	dScene::dTreeNode* const meshNode = scene.CreateMeshNode(parentNode);
	dMeshNodeInfo* const instance = (dMeshNodeInfo*) scene.GetInfoFromNode(meshNode);

	char name[256];
	sprintf (name, "%s_mesh", parent->GetName());
	instance->SetName(name);


//	maxMesh.buildNormals();
	dMatrix objectMatrix (GetMatrixFromMaxMatrix (node->GetObjectTM(0)));
	dMatrix nodeMatrix (parent->GetTransform());
	dMatrix scaleTransform2 (objectMatrix * nodeMatrix.Inverse4x4());

	dVector scale;
	dMatrix matrix;
	dMatrix stretchAxis;
	scaleTransform2.PolarDecomposition (matrix, scale, stretchAxis);
	dMatrix scaleTransform (dMatrix (GetIdentityMatrix(), scale, stretchAxis));

	instance->SetPivotMatrix(matrix);


	int* const faceIndexCount = new int [facesCount];
	int* const materialIndex = new int [facesCount];

	Mtl* const mtl = node->GetMtl();
	int facesMasked = facesCount;
	memset (materialIndex, -1, facesCount * sizeof (int));

	if (mtl) {
		int subMatCount = mtl->NumSubMtls() ? mtl->NumSubMtls() : 1;

		for (int i = 0; i < subMatCount; i ++) {
			Mtl* const maxMaterial = mtl->NumSubMtls() ? mtl->GetSubMtl(i) : mtl;

			bool materialIsUsed = false;
			for (int j = 0; j < facesCount; j ++) {
				MNFace* const maxFace = maxMesh.F(j);
				if (maxFace->material == i) {
					materialIsUsed = true;
					facesMasked --;
					materialIndex[j] = materialID;
				}
			}

			if (materialIsUsed) {
				dScene::dTreeNode* const matNode = scene.CreateMaterialNode(meshNode, materialID);
				dMaterialNodeInfo* const material = (dMaterialNodeInfo*) scene.GetInfoFromNode(matNode);

				materialID ++;
				if (mtl->NumSubMtls()) {
					//MSTR materialName = maxMaterial->GetSubMtlSlotName(i);
					MSTR materialName = mtl->GetName();
					char name[256];
					sprintf (name, "%s_%d", materialName, i);
					//material->SetName(materialName);
					material->SetName(name);
				} else {
					MSTR materialName = mtl->GetName();
					material->SetName(materialName);
				}

				Color ambient (maxMaterial->GetAmbient());
				Color difusse (maxMaterial->GetDiffuse());
				Color specular (maxMaterial->GetSpecular());
				float shininess (maxMaterial->GetShininess());
				float shininessStr (maxMaterial->GetShinStr());      
				float tranparency (maxMaterial->GetXParency());

				material->SetAmbientColor (dVector (ambient.r, ambient.g, ambient.b, 1.0f));
				material->SetDiffuseColor (dVector (difusse.r, difusse.g, difusse.b, 1.0f));
				material->SetSpecularColor(dVector (specular.r * shininess, specular.g * shininess, specular.b * shininess, 1.0f));
				material->SetShininess(shininessStr * 100.0f);
				material->SetOpacity (1.0f - tranparency);

				Texmap* const tex = maxMaterial->GetSubTexmap(1);
				if (tex) {
					Class_ID texId (tex->ClassID());
					if (texId != Class_ID(BMTEX_CLASS_ID, 0x00)) {
						_ASSERTE (0);
						//					continue;
					}
					BitmapTex* const bitmapTex = (BitmapTex *)tex;
					const char* const texPathName = bitmapTex->GetMapName();

					const char* texName = strrchr (texPathName, '\\');
					if (texName) {
						texName ++;
					} else {
						texName = strrchr (texPathName, '/');
						if (texName) {
							texName ++;
						} else {
							texName = texPathName;
						}
					}

					int crc = dCRC(texName);
					dTree<dScene::dTreeNode*, int>::dTreeNode* cacheNode = imageCache.Find(crc);
					if (!cacheNode) {
						dScene::dTreeNode* textNode = scene.CreateTextureNode(texName);
						cacheNode = imageCache.Insert(textNode, crc);
					}
					dScene::dTreeNode* const textNode = cacheNode->GetInfo();
					scene.AddReference(matNode, textNode);
					dTextureNodeInfo* const texture = (dTextureNodeInfo*) scene.GetInfoFromNode(textNode);
					//texture->SetName("texture");
					texture->SetName(texName);
					material->SetDiffuseTextId(texture->GetId());
					material->SetAmbientTextId(texture->GetId());
					material->SetSpecularTextId(texture->GetId());
				}
			}
		}

		if (facesMasked) {
			dScene::dTreeNode* const matNode = scene.CreateMaterialNode(meshNode, materialID);
			for (int j = 0; j < facesCount; j ++) {
				if (materialIndex[j] == -1) {
					materialIndex[j] = materialID;
				}
			}
			materialID ++;
			dMaterialNodeInfo* const material = (dMaterialNodeInfo*) scene.GetInfoFromNode(matNode);
			material->SetName("default material");
		}
	} else {

		dScene::dTreeNode* const matNode = scene.CreateMaterialNode(meshNode, materialID);
		for (int j = 0; j < facesCount; j ++) {
			materialIndex[j] = materialID;
		}
		materialID ++;
		dMaterialNodeInfo* const material = (dMaterialNodeInfo*) scene.GetInfoFromNode(matNode);
		material->SetName("default material");
	}


	int indexCount = 0;
	for (int i = 0; i < facesCount; i ++) {
		MNFace* const face = maxMesh.F(i);
		faceIndexCount[i] = face->deg;
		indexCount += face->deg;
	}

	int* const vertexIndex = new int[vertexCount];
	dVector* const vertex = new dVector[vertexCount];
	for (int i = 0; i < vertexCount; i ++) {
		vertexIndex[i] = i;
		Point3 p (maxMesh.P(i));
		vertex[i] = scaleTransform.TransformVector (dVector (p.x, p.y, p.z, 0.0f));
	}
	vertexCount = dPackVertexArray (&vertex[0].m_x, 3, sizeof (dVector), vertexCount, vertexIndex);


	dVector* UV0 = NULL;
	MNMapFace* tvFaceArray = NULL;  	 

	int uvCount = 1;
	if (maxMesh.M(1)) {
		MNMap* const mapChannel = maxMesh.M(1);
		uvCount = mapChannel->VNum();
		if (uvCount) {
			tvFaceArray = mapChannel->f;
			UVVert* const tVertsArray = mapChannel->v;
			UV0 = new dVector[uvCount];
			for (int i = 0; i < uvCount; i ++) {
				UVVert& uv = tVertsArray[i];
				UV0[i] = dVector (uv.x, uv.y, 0.0f, 0.0f);
			}
		} else {
			uvCount = 1;
			UV0 = new dVector[1];
			UV0[0] = dVector (0.0f, 0.0f, 0.0f, 0.0f);
		}
	}


	dVector* const normal = new dVector[indexCount];
	dVector UV1 (0.0f, 0.0f, 0.0f, 0.0f);

	int* const uv0List = new int [indexCount]; 
	int* const uv1List = new int [indexCount]; 
	int* const normalList = new int [indexCount];
	int* const vertexList = new int [indexCount];

	int index = 0;
	dMatrix normalMatrix (scaleTransform.Inverse4x4().Transpose() * scaleTransform);
	scaleTransform.m_posit = dVector (0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < facesCount; i ++) {
		MNFace* const maxFace = maxMesh.F(i);
		MNMapFace* tvFace = NULL;
		if (tvFaceArray) {
			tvFace = &tvFaceArray[i];  	 
			_ASSERTE (tvFace->deg == maxFace->deg);
		}

		for (int j = 0; j < maxFace->deg; j ++) {

			vertexList[index] = vertexIndex[maxFace->vtx[j]];

//			dVector n (normalMatrix.RotateVector(CalcVertexNormal (maxMesh, i, j)));
//			normal[index] = n.Scale (1.0f / dSqrt (n % n));
//			normalList[index] = index;
			normal[0] = dVector (0.0f, 0.0f, 0.0f, 0.0f);
			normalList[index] = 0;

			uv0List[index] = 0;
			if (tvFaceArray) {
				uv0List[index] = tvFace->tv[j];
			}

			uv1List[index] = 0;

			index ++;
		}
	}


	instance->BuildFromVertexListIndexList(facesCount, faceIndexCount, materialIndex, 
										   &vertex[0].m_x, sizeof (dVector), vertexList,
										   &normal[0].m_x, sizeof (dVector), normalList,
										   &UV0[0].m_x, sizeof (dVector), uv0List,
										   &UV1.m_x, sizeof (dVector), uv1List);

	instance->SmoothNormals (3.1416f * 45.0f / 180.0f);

	delete[] vertexIndex;
	delete[] UV0;
	delete[] normal;
	delete[] vertex;
	delete[] uv1List; 
	delete[] uv0List; 
	delete[] normalList;
	delete[] vertexList;
	delete[] materialIndex;
	delete[] faceIndexCount;
	if (needDel) {
		delete poly;
	}

	// get the object reference of the node
	Modifier* modifier = NULL;
	Object* pObject = node->GetObjectRef();
	if(pObject) {

		// loop through all derived objects
		while((pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID))
		{
			IDerivedObject* const pDerivedObject = static_cast<IDerivedObject *>(pObject);

			// loop through all modifiers
			for(int stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
			{
				// get the modifier
				Modifier* const pModifier = pDerivedObject->GetModifier(stackId);

				// check if we found the skin modifier
				if(pModifier->ClassID() == SKIN_CLASSID) {
					modifier = pModifier;
					break;
				}
			}

			if (modifier) {
				break;
			}
			// continue with next derived object
			pObject = pDerivedObject->GetObjRef();
		}
	}

	if (modifier) {
		// create a skin interface
		ISkin* const pSkin = (ISkin*)modifier->GetInterface(I_SKIN);
		_ASSERTE (pSkin);

		// create a skin context data interface
		ISkinContextData* const pSkinContextData = (ISkinContextData *)pSkin->GetContextInterface(node);
		_ASSERTE (pSkinContextData);


		int weightDataCount = 0;
		dScene::dTreeNode* const skinNode = scene.CreateSkinModifierNode(meshNode);
		dGeometryNodeSkinModifierInfo::dBoneVertexWeightData* const skindata = new dGeometryNodeSkinModifierInfo::dBoneVertexWeightData[vertexCount * 4];

		dMeshNodeInfo* const info = (dMeshNodeInfo*) scene.GetInfoFromNode (meshNode);
		NewtonMesh* const mesh = info->GetMesh();
		int stride = NewtonMeshGetVertexStrideInByte(mesh) / sizeof (dFloat64);
		int vCount = NewtonMeshGetVertexCount(mesh);
		const dFloat64* const vertex = NewtonMeshGetVertexArray(mesh);

		int layersCount = 1;
		for (int i = 0; i < vCount; i ++) {
			layersCount = max (layersCount, int (vertex[i * stride + 3]) + 1);
		}
		int vertexBaseCount = vertexCount / layersCount;
		_ASSERTE (vertexBaseCount == vertexCount);
		for (int layer = 0; layer < layersCount; layer ++) {
			for (int i = 0; i < vertexCount; i ++) {
				int nodeIDCount = pSkinContextData->GetNumAssignedBones(i);
				for(int nodeId = 0; nodeId < nodeIDCount; nodeId++) {
					int boneId = pSkinContextData->GetAssignedBone(i, nodeId);
					dFloat weight = pSkinContextData->GetBoneWeight(i, nodeId);
					INode* const maxBone = pSkin->GetBone(boneId);
					_ASSERTE (nodeMap.Find(maxBone));
					dScene::dTreeNode* const bone = nodeMap.Find(maxBone)->GetInfo();

					if (dFloat (weight) > 1.0e-6f) {
						skindata[weightDataCount].m_boneNode = bone;
						skindata[weightDataCount].m_weight = weight;
						skindata[weightDataCount].m_vertexIndex = i + layer * vertexBaseCount;
						weightDataCount ++;
					}
				}
			}
		}

		dGeometryNodeSkinModifierInfo* const skinInfo = (dGeometryNodeSkinModifierInfo*) scene.GetInfoFromNode(skinNode);
		skinInfo->SkinMesh(skinNode, &scene, skindata, weightDataCount);
		delete[] skindata;
	}

	return meshNode;

}

#else 


dVector Export::CalcVertexNormal (Mesh& mesh, int faceNo, int faceIndex) const
{
	Point3 normal;
	dVector vertexNormal (0.0f, 0.0f, 0.0f, 0.0f);

	Face* const face = &mesh.faces[faceNo];

	int vertexIndex = face->v[faceIndex];

	DWORD smGroup = face->getSmGroup();
	RVertex* const rv	= mesh.getRVertPtr(vertexIndex);

	int numNormals = rv->rFlags & NORCT_MASK;
	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) {
		_ASSERTE (0);
		normal = rv->rn.getNormal();
		vertexNormal.m_x = normal.x;
		vertexNormal.m_y = normal.y;
		vertexNormal.m_z = normal.z;

	} else if (numNormals && smGroup) {
		// If normal is not specified it's only available if the face belongs
		// to a smoothing group

		if (numNormals == 1) {
			// If there is only one vertex is found in the rn member.
			normal = rv->rn.getNormal();
			vertexNormal.m_x = normal.x;
			vertexNormal.m_y = normal.y;
			vertexNormal.m_z = normal.z;

		} else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) {
				if (rv->ern[i].getSmGroup() & smGroup) {
					normal = rv->ern[i].getNormal();
					vertexNormal.m_x += normal.x;
					vertexNormal.m_y += normal.y;
					vertexNormal.m_z += normal.z;
				}
			}
		}
	} else {
		// Get the normal from the Face if no smoothing groups are there
		normal = max5Mesh.getFaceNormal(faceNo);
		vertexNormal.m_x = normal.x;
		vertexNormal.m_y = normal.y;
		vertexNormal.m_z = normal.z;
	}

	return vertexNormal;
}



TriObject* Export::GetTriObject (ObjectState* const os, int& deleteIt) 
{
	Object *obj;
	TriObject *poly;

	poly = NULL;
	deleteIt = FALSE;
	obj = os->obj;
	if (obj->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0))) { 
		poly = (TriObject *) obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != poly) {
			deleteIt = TRUE;
		}
	}
	return poly;
}


dScene::dTreeNode* Export::LoadTriangleObject(INode* const node, ObjectState* const os, dScene& scene, NodeMap& nodeMap, ImageCache& imageCache, int& materialID)
{
	// Targets are actually geomobjects, but we will export them
	// from the camera and light objects, so we skip them here.
	if (os->obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0)) {
		return NULL;
	}
	
	BOOL needDel = FALSE;
	TriObject* const poly = GetTriObject (os, needDel); 
	if (!poly) {
		return NULL;
	}

	Mesh& maxMesh = poly->GetMesh();
	int facesCount = maxMesh.getNumFaces();
	if (!facesCount) {
		if (needDel) {
			delete poly;
		}
		return NULL;
	}

	_ASSERTE (nodeMap.Find (node));
	dScene::dTreeNode* const parentNode = nodeMap.Find (node)->GetInfo();
	dSceneNodeInfo* const parent = (dSceneNodeInfo*) scene.GetInfoFromNode(parentNode);


	dScene::dTreeNode* const meshNode = scene.CreateMeshNode(parentNode);
	dMeshNodeInfo* const instance = (dMeshNodeInfo*) scene.GetInfoFromNode(meshNode);

	char name[256];
	sprintf (name, "%s_mesh", parent->GetName());
	instance->SetName(name);


	maxMesh.buildNormals();
	dMatrix objectMatrix (GetMatrixFromMaxMatrix (node->GetObjectTM(0)));
	dMatrix nodeMatrix (parent->GetTransform());
	dMatrix scaleTransform2 (objectMatrix * nodeMatrix.Inverse4x4());

	dVector scale;
	dMatrix matrix;
	dMatrix stretchAxis;
	scaleTransform2.PolarDecomposition (matrix, scale, stretchAxis);
	dMatrix scaleTransform (dMatrix (GetIdentityMatrix(), scale, stretchAxis));

//scaleTransform = scaleTransform2;
//matrix = GetIdentityMatrix();

	instance->SetPivotMatrix(matrix);

	Face* const maxFaceArray = maxMesh.faces;
	Point3* const maxVertexArray = maxMesh.verts;
	UVVert* const tVertsArray = maxMesh.tVerts;
	TVFace* const tvFaceArray = maxMesh.tvFace;  	 

	int vertexCount = maxMesh.getNumVerts();
	int uvCount = maxMesh.getNumTVerts();
	if (uvCount == 0) {
		uvCount = 1;
	}

	int* const faceIndexCount = new int [facesCount];
	int* const materialIndex = new int [facesCount];
	int* const vertexList = new int [facesCount * 3];
	int* const normalList = new int [facesCount * 3];
	int* const uv0List = new int [facesCount * 3]; 
	int* const uv1List = new int [facesCount * 3]; 


	Mtl* const mtl = node->GetMtl();
	int facesMasked = facesCount;
	memset (materialIndex, -1, facesCount * sizeof (int));

	if (mtl) {
		int subMatCount = mtl->NumSubMtls() ? mtl->NumSubMtls() : 1;

		for (int i = 0; i < subMatCount; i ++) {
			Mtl* const maxMaterial = mtl->NumSubMtls() ? mtl->GetSubMtl(i) : mtl;
			
			bool materialIsUsed = false;
			for (int j = 0; j < facesCount; j ++) {
				Face* const maxFace = &maxFaceArray[j];
				if (maxFace->getMatID() == i) {
					materialIsUsed = true;
					facesMasked --;
					materialIndex[j] = materialID;
				}
			}

			if (materialIsUsed) {
				dScene::dTreeNode* const matNode = scene.CreateMaterialNode(meshNode, materialID);
				dMaterialNodeInfo* const material = (dMaterialNodeInfo*) scene.GetInfoFromNode(matNode);

				materialID ++;
				if (mtl->NumSubMtls()) {
					//MSTR materialName = maxMaterial->GetSubMtlSlotName(i);
					MSTR materialName = mtl->GetName();
					char name[256];
					sprintf (name, "%s_%d", materialName, i);
					//material->SetName(materialName);
					material->SetName(name);
				} else {
					MSTR materialName = mtl->GetName();
					material->SetName(materialName);
				}

				Color ambient (maxMaterial->GetAmbient());
				Color difusse (maxMaterial->GetDiffuse());
				Color specular (maxMaterial->GetSpecular());
				float shininess (maxMaterial->GetShininess());
				float shininessStr (maxMaterial->GetShinStr());      
				float tranparency (maxMaterial->GetXParency());

				material->SetAmbientColor (dVector (ambient.r, ambient.g, ambient.b, 1.0f));
				material->SetDiffuseColor (dVector (difusse.r, difusse.g, difusse.b, 1.0f));
				material->SetSpecularColor(dVector (specular.r * shininess, specular.g * shininess, specular.b * shininess, 1.0f));
				material->SetShininess(shininessStr * 100.0f);
				material->SetOpacity (1.0f - tranparency);

				Texmap* const tex = maxMaterial->GetSubTexmap(1);
				if (tex) {
					Class_ID texId (tex->ClassID());
					if (texId != Class_ID(BMTEX_CLASS_ID, 0x00)) {
						_ASSERTE (0);
	//					continue;
					}
					BitmapTex* const bitmapTex = (BitmapTex *)tex;
					const char* const texPathName = bitmapTex->GetMapName();

					const char* texName = strrchr (texPathName, '\\');
					if (texName) {
						texName ++;
					} else {
						texName = strrchr (texPathName, '/');
						if (texName) {
							texName ++;
						} else {
							texName = texPathName;
						}
					}

					int crc = dCRC(texName);
					dTree<dScene::dTreeNode*, int>::dTreeNode* cacheNode = imageCache.Find(crc);
					if (!cacheNode) {
						dScene::dTreeNode* textNode = scene.CreateTextureNode(texName);
						cacheNode = imageCache.Insert(textNode, crc);
					}
					dScene::dTreeNode* const textNode = cacheNode->GetInfo();
					scene.AddReference(matNode, textNode);
					dTextureNodeInfo* const texture = (dTextureNodeInfo*) scene.GetInfoFromNode(textNode);
					//texture->SetName("texture");
					texture->SetName(texName);
					material->SetDiffuseTextId(texture->GetId());
					material->SetAmbientTextId(texture->GetId());
					material->SetSpecularTextId(texture->GetId());
				}
			}
		}

		if (facesMasked) {
			dScene::dTreeNode* const matNode = scene.CreateMaterialNode(meshNode, materialID);
			for (int j = 0; j < facesCount; j ++) {
				if (materialIndex[j] == -1) {
					materialIndex[j] = materialID;
				}
			}
			materialID ++;
			dMaterialNodeInfo* const material = (dMaterialNodeInfo*) scene.GetInfoFromNode(matNode);
			material->SetName("default material");
		}
	} else {
		
		dScene::dTreeNode* const matNode = scene.CreateMaterialNode(meshNode, materialID);
		for (int j = 0; j < facesCount; j ++) {
			materialIndex[j] = materialID;
		}
		materialID ++;
		dMaterialNodeInfo* const material = (dMaterialNodeInfo*) scene.GetInfoFromNode(matNode);
		material->SetName("default material");
	}


	dVector* const vertex = new dVector[vertexCount];
	dVector* const normal = new dVector[facesCount * 3];
	dVector* const UV0 = new dVector[uvCount];
	dVector UV1 (0.0f, 0.0f, 0.0f, 0.0f);

	int* const vertexIndex = new int[vertexCount];
	for (int i = 0; i < vertexCount; i ++) {
		vertexIndex[i] = i;
		vertex[i] = scaleTransform.TransformVector (dVector (maxVertexArray[i].x, maxVertexArray[i].y, maxVertexArray[i].z, 0.0f));
	}
	vertexCount = dPackVertexArray (&vertex[0].m_x, 3, sizeof (dVector), vertexCount, vertexIndex);

	memset (UV0, 0, uvCount * sizeof (dVector));
	if (tVertsArray) {
		for (int i = 0; i < uvCount; i ++) {
			UVVert& uv = tVertsArray[i];
			UV0[i] = dVector (uv.x, uv.y, 0.0f, 0.0f);
		}
	}
	

	dMatrix normalMatrix (scaleTransform.Inverse4x4().Transpose() * scaleTransform);
	scaleTransform.m_posit = dVector (0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < facesCount; i ++) {
		Face* const maxFace = &maxFaceArray[i];
		TVFace* tvFace = NULL;
		if (tvFaceArray) {
			tvFace = &tvFaceArray[i];  	 
		}
		faceIndexCount[i] = 3;
		for (int j = 0; j < 3; j ++) {
			int m = i * 3 + j;

			dVector n (normalMatrix.RotateVector(CalcVertexNormal (maxMesh, i, j)));
			normal[m] = n.Scale (1.0f / dSqrt (n % n));

			vertexList[m] = vertexIndex[maxFace->v[j]];
			normalList[m] = m;
			uv0List[m] = 0;
			if (tvFace) {
				uv0List[m] = tvFace->getTVert(j);
			}
			uv1List[m] = 0;
		}
	}
	
	instance->BuildFromVertexListIndexList(facesCount, faceIndexCount, materialIndex, 
										   &vertex[0].m_x, sizeof (dVector), vertexList,
										   &normal[0].m_x, sizeof (dVector), normalList,
										   &UV0[0].m_x, sizeof (dVector), uv0List,
										   &UV1.m_x, sizeof (dVector), uv1List);

	delete[] vertexIndex;
	delete[] UV0;
	delete[] normal;
	delete[] vertex;
	delete[] uv1List; 
	delete[] uv0List; 
	delete[] normalList;
	delete[] vertexList;
	delete[] materialIndex;
	delete[] faceIndexCount;
	if (needDel) {
		delete poly;
	}

	// get the object reference of the node
	Modifier* modifier = NULL;
	Object* pObject = node->GetObjectRef();
	if(pObject) {

		// loop through all derived objects
		while((pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID))
		{
			IDerivedObject* const pDerivedObject = static_cast<IDerivedObject *>(pObject);

			// loop through all modifiers
			for(int stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
			{
				// get the modifier
				Modifier* const pModifier = pDerivedObject->GetModifier(stackId);

				// check if we found the skin modifier
				if(pModifier->ClassID() == SKIN_CLASSID) {
					modifier = pModifier;
					break;
				}
			}

			if (modifier) {
				break;
			}
			// continue with next derived object
			pObject = pDerivedObject->GetObjRef();
		}
	}

	if (modifier) {
		// create a skin interface
		ISkin* const pSkin = (ISkin*)modifier->GetInterface(I_SKIN);
		_ASSERTE (pSkin);

		// create a skin context data interface
		ISkinContextData* const pSkinContextData = (ISkinContextData *)pSkin->GetContextInterface(node);
		_ASSERTE (pSkinContextData);


		int weightDataCount = 0;
		dScene::dTreeNode* const skinNode = scene.CreateSkinModifierNode(meshNode);
		dGeometryNodeSkinModifierInfo::dBoneVertexWeightData* const skindata = new dGeometryNodeSkinModifierInfo::dBoneVertexWeightData[vertexCount * 4];

		dMeshNodeInfo* const info = (dMeshNodeInfo*) scene.GetInfoFromNode (meshNode);
		NewtonMesh* const mesh = info->GetMesh();
		int stride = NewtonMeshGetVertexStrideInByte(mesh) / sizeof (dFloat64);
		int vCount = NewtonMeshGetVertexCount(mesh);
		const dFloat64* const vertex = NewtonMeshGetVertexArray(mesh);

		int layersCount = 1;
		for (int i = 0; i < vCount; i ++) {
			layersCount = max (layersCount, int (vertex[i * stride + 3]) + 1);
		}
		int vertexBaseCount = vertexCount / layersCount;
		_ASSERTE (vertexBaseCount == vertexCount);
		for (int layer = 0; layer < layersCount; layer ++) {
			for (int i = 0; i < vertexCount; i ++) {
				int nodeIDCount = pSkinContextData->GetNumAssignedBones(i);
				for(int nodeId = 0; nodeId < nodeIDCount; nodeId++) {
					int boneId = pSkinContextData->GetAssignedBone(i, nodeId);
					dFloat weight = pSkinContextData->GetBoneWeight(i, nodeId);
					INode* const maxBone = pSkin->GetBone(boneId);
					_ASSERTE (nodeMap.Find(maxBone));
					dScene::dTreeNode* const bone = nodeMap.Find(maxBone)->GetInfo();

					if (dFloat (weight) > 1.0e-6f) {
						skindata[weightDataCount].m_boneNode = bone;
						skindata[weightDataCount].m_weight = weight;
						skindata[weightDataCount].m_vertexIndex = i + layer * vertexBaseCount;
						weightDataCount ++;
					}
				}
			}
		}

		dGeometryNodeSkinModifierInfo* const skinInfo = (dGeometryNodeSkinModifierInfo*) scene.GetInfoFromNode(skinNode);
		skinInfo->SkinMesh(skinNode, &scene, skindata, weightDataCount);
		delete[] skindata;
	}
	return meshNode;
}
#endif

Export::Export(const char* const pathName, ExpInterface* const expip, Interface* const ip, const Options& options)
{
	m_ip = ip;
	m_expip = expip;

//	int type;
	float scale;
//	GetMasterUnitInfo(&type, &scale);
//	switch (type)
//#define UNITS_INCHES		0
//#define UNITS_FEET		1
//#define UNITS_MILES		2
//#define UNITS_MILLIMETERS	3
//#define UNITS_CENTIMETERS	4
//#define UNITS_METERS		5
//#define UNITS_KILOMETERS	6
	scale = float (GetMasterScale(UNITS_METERS));


	NewtonWorld* newton = NewtonCreate();
	dScene scene (newton);
	if (options.m_exportSkeleton || options.m_exportMesh) {
		LoadSkeletonAndGeomtry (m_ip->GetRootNode(), scene);
/*
		if (options.m_exportAnimation) {
			IMixer8 *mixer = NULL;
			for (int i = 0; i < TheMaxMixerManager.NumMaxMixers(); i ++ ) {
				mixer = TheMaxMixerManager.GetNthMaxMixer(i);
				if (mixer) {
					break;
				}
			}

			if (mixer) {
//				Tab<INode *> nodeList;
//				GetNodeList (nodeList);
				int trackGroudCount = mixer->NumTrackgroups();
				IMXtrackgroup* trackGroup = mixer->GetTrackgroup(0);
				int tracksCount = trackGroup->NumTracks();

				// mute all animations;
				for (int i = 0; i < tracksCount; i ++) {
					IMXtrack *track = trackGroup->GetTrack(i);
					track->SetMute(true);
				}

				// now enable clips one at a time
				for (int i = 0; i < tracksCount; i ++) {
					IMXtrack *track = trackGroup->GetTrack(i);
					track->SetMute(false);

					char name[256];
					sprintf (name, "clip%02d", i);

					int clipCount = track->NumClips(BOT_ROW);
					for (int j = 0; j < clipCount; j ++) {
						IMXclip *clip = track->GetClip(j);
						const char* name1 = clip->GetFilename();
						name1 = clip->GetFilename();

						const char* ptr = strrchr (name1, '\\');
						if (!ptr) {
							ptr = name1;
						} else {
							ptr ++;
						}
						sprintf (name, "%s", ptr);
						strtok (name, ".");
					}

					AnimationClip animation;

					dAnimationClip* clip = new dAnimationClip;
					strcpy (clip->m_name, name);

					TimeValue start (m_ip->GetAnimRange().Start());
					TimeValue end (m_ip->GetAnimRange().End());

					if ((end - start) > 0.0f) {
						animation.m_start = start;
						animation.m_end = end;
						animation.m_fps = GetTicksPerFrame();
						animation.Load (m_ip->GetRootNode(), clip);

						// find if this animation is use by this model
						if (clip->GetCount()) {
							dAnimationClip::dListNode* animNode = clip->GetFirst();
							
							const char* animNodeName = animNode->GetInfo().m_bindName;
							int nameLegnth = strlen (animNodeName);
							
							for (dSceneModelList::dListNode* node = modelList.GetFirst(); node; node = node->GetNext()) {
								dList<dBone*> boneList;
								dModel* model = node->GetInfo();
								
								model->GetBoneList (boneList);
								for (dList<dBone*>::dListNode* boneNode = boneList.GetFirst(); boneNode; boneNode = boneNode->GetNext()) {
									if (!strncmp (boneNode->GetInfo()->GetName(), animNodeName, nameLegnth)) {
										model->AddAnimation(clip);
										break;
									}
								}
							}
						}
					}

					clip->Release();
					track->SetMute(true);
				}
			}
		}
*/
	}


	dMatrix scaleMatrix (GetIdentityMatrix());
	scaleMatrix[0][0] = scale;
	scaleMatrix[1][1] = scale;
	scaleMatrix[2][2] = scale;
	dMatrix matrix (scaleMatrix * dPitchMatrix(-3.14159265f * 0.5f) * dYawMatrix(-3.14159265f * 0.5f)); 

	scene.BakeTransform (matrix);
	scene.RemoveUnusedVertex();
	scene.Serialize (pathName);

	scene.CleanUp();
	NewtonDestroy(newton);
}

Export::~Export(void)
{
}
