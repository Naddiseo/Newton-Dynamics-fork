
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
#include "import.h"
#include "ImportDesc.h"



class MaxNodeChache: public dTree<INode*, const dScene::dTreeNode* >
{

};


class GeometryCache: public dTree<GeomObject*, const dScene::dTreeNode* >
{
	public:
	void AddMesh (GeomObject* geometry, const dScene::dTreeNode* mesh)
	{
		Insert (geometry, mesh);
	}
};

class MaterialProxi
{
	public: 
	Mtl* m_mtl;
	int m_matID;
};


class MaterialCache: public dTree<MaterialProxi, int>
{
	public:
	MaterialCache(MultiMtl* const multiMat)
		:dTree<MaterialProxi, int>()
	{
		m_matID = 0;
		m_multiMat = multiMat;
		m_multiMat->SetMtlFlag(MTL_DISPLAY_ENABLE_FLAGS);
		m_multiMat->SetName("default");
	}

	//	void SetSubMaterialCount(int count)
	//	{
	//		m_multiMat->SetNumSubMtls(count);
	//	}

	const MaterialProxi* FindMaterial (int id) const
	{
		dTreeNode* const node = Find (id);
		if (node) {
			return &node->GetInfo();
		} else {
			return NULL;
		}
	}

	void AddMaterial (const MaterialProxi& material, int id)
	{
		MaterialProxi tmp (material);

		tmp.m_matID = m_matID; 
		Insert (tmp, id);
		m_multiMat->SetSubMtl(m_matID, tmp.m_mtl);
		tmp.m_mtl->SetMtlFlag(MTL_DISPLAY_ENABLE_FLAGS);
		m_matID ++;
	}


	int m_matID;
	MultiMtl* m_multiMat;
};


void Import::SetSmoothingGroups (Mesh& maxMesh)
{
	struct FaceAdjecency
	{
		int m_count;
		int m_adjacentFace[9];
	};


	int edgeCount;
	int faceIndexPool[1024 * 8];
	int triangleCount = maxMesh.getNumFaces(); 

	maxMesh.InvalidateTopologyCache();
	maxMesh.InvalidateGeomCache();
	Edge* const edgeList = maxMesh.MakeEdgeList(&edgeCount);

	FaceAdjecency* const adjacency = new FaceAdjecency [triangleCount];
	dVector* const faceNormals = new dVector[triangleCount];

	memset (adjacency, 0, triangleCount * sizeof (FaceAdjecency));
	for (int i = 0; i < edgeCount; i ++) {
		int face0 = edgeList[i].f[0];
		int face1 = edgeList[i].f[1];
		if ((face0 != -1) && (face1 != -1)) {
			_ASSERTE (face0 < triangleCount);
			_ASSERTE (face1 < triangleCount);

			adjacency[face0].m_adjacentFace[adjacency[face0].m_count] = face1;
			adjacency[face1].m_adjacentFace[adjacency[face1].m_count] = face0;

			adjacency[face0].m_count += 1;
			adjacency[face1].m_count += 1;

			_ASSERTE (adjacency[face0].m_count <= sizeof (adjacency[0].m_adjacentFace) / sizeof (adjacency[0].m_adjacentFace[0]));
			_ASSERTE (adjacency[face1].m_count <= sizeof (adjacency[0].m_adjacentFace) / sizeof (adjacency[0].m_adjacentFace[0]));
		}
	}

	for (int i = 0; i < triangleCount; i ++) {
		Face* face;
		face = &maxMesh.faces[i];
		dVector p0 (maxMesh.verts[face->v[0]].x, maxMesh.verts[face->v[0]].y, maxMesh.verts[face->v[0]].z, 0.0f);
		dVector p1 (maxMesh.verts[face->v[1]].x, maxMesh.verts[face->v[1]].y, maxMesh.verts[face->v[1]].z, 0.0f);
		dVector p2 (maxMesh.verts[face->v[2]].x, maxMesh.verts[face->v[2]].y, maxMesh.verts[face->v[2]].z, 0.0f);

		dVector normal ((p1 - p0) * (p2 - p0));
		faceNormals[i] = normal.Scale (1.0f / dSqrt (normal % normal));
	}


	unsigned group = 1;
	for (int i = 0; i < triangleCount; i ++) {
		Face* const face = &maxMesh.faces[i];
		if (!face->smGroup) {

			face->setSmGroup(group);
			faceIndexPool[0] = i;
			int stack = 1;

			while (stack) {
				stack --;
				int index = faceIndexPool[stack];

				dVector& n0 = faceNormals[index];
				for (int j = 0; j < adjacency[index].m_count; j ++) {
					int adjacentFaceIndex = adjacency[index].m_adjacentFace[j];
					Face* const adjacentFace = &maxMesh.faces[adjacentFaceIndex];

					if (!adjacentFace->smGroup) {
						dVector& n1 = faceNormals[adjacentFaceIndex];

						float dot = n0 % n1;
						if (dot > 0.86f) {
							if (stack < sizeof (faceIndexPool) / sizeof (faceIndexPool[0])) {
								adjacentFace->setSmGroup(group);
								faceIndexPool[stack] = adjacentFaceIndex;
								stack ++;
							}
						}
					}
				}
			}

			group = group * 2;
			if (!group) {
				group = 1;
			}
		}
	}


	delete[] faceNormals;
	delete[] adjacency;

	maxMesh.buildNormals();
}



void Import::LoadMaterials (dScene& scene, MaterialCache& materialCache)
{
	dScene::Iterator iter (scene);
	for (iter.Begin(); iter; iter ++) {
		dScene::dTreeNode* const materialNode = iter.GetNode();
		dNodeInfo* const info = scene.GetInfoFromNode(materialNode);
		if (info->IsType(dMaterialNodeInfo::GetRttiType())) {
			MaterialProxi material;
			material.m_mtl = NewDefaultStdMat();
			StdMat* const stdMtl = (StdMat*)material.m_mtl;

			dMaterialNodeInfo* const materialInfo = (dMaterialNodeInfo*) info;
			stdMtl->SetName(materialInfo->GetName());

			dVector ambient (materialInfo->GetAmbientColor());
			dVector difusse (materialInfo->GetDiffuseColor());
			dVector specular (materialInfo->GetSpecularColor());
			float shininess (materialInfo->GetShininess());
			//float shininessStr (materialInfo->GetShinStr());      
			float transparency (materialInfo->GetOpacity());

			stdMtl->SetAmbient(*((Point3*)&ambient), 0);
			stdMtl->SetDiffuse(*((Point3*)&difusse), 0);
			stdMtl->SetSpecular(*((Point3*)&specular), 0);
			stdMtl->SetShinStr(shininess / 100.0f, 0);      
			stdMtl->SetOpacity(transparency, 0);


			if (materialInfo->GetDiffuseTextId() != -1) {
				dScene::dTreeNode* textNode = scene.FindTextureByTextId(materialNode, materialInfo->GetDiffuseTextId());
				if (textNode) {
					_ASSERTE (textNode);
	//				BitmapTex* bmtex;
	//				const TCHAR* txtName;

					dTextureNodeInfo* textureInfo = (dTextureNodeInfo*) scene.GetInfoFromNode(textNode);
					TCHAR txtNameBuffer[256];
					sprintf (txtNameBuffer, "%s/%s", m_path, textureInfo->GetPathName());

					const TCHAR* txtName = txtNameBuffer;
					BitmapTex* bmtex = (BitmapTex*)NewDefaultBitmapTex();
					bmtex->SetMapName((TCHAR*)txtName);

					txtName = textureInfo->GetPathName();
					bmtex->SetName (txtName);
					bmtex->GetUVGen()->SetMapChannel(1);

					stdMtl->SetSubTexmap(ID_DI, bmtex);
					stdMtl->SetTexmapAmt(ID_DI, 1.0f, 0);
					stdMtl->EnableMap(ID_DI, TRUE);

	//					const char* materialOpanacity = segment.m_opacityTextureName;
	//					if (materialOpanacity[0]) {
	//						BitmapTex* bmtex;
	//						const TCHAR* txtName;
	//
	//						txtName = segment.m_opacityPathName;
	//						bmtex = (BitmapTex*)NewDefaultBitmapTex();
	//						bmtex->SetMapName((TCHAR*)txtName);
	//
	//						txtName = materialName;
	//						bmtex->SetName (txtName);
	//						bmtex->GetUVGen()->SetMapChannel(2);
	//
	//						stdMtl->SetSubTexmap(ID_OP, bmtex);
	//						stdMtl->SetTexmapAmt(ID_OP, 1.0f, 0);
	//						stdMtl->EnableMap(ID_OP, TRUE);
	//					}
	//				materialCache.AddMaterial(material, segment.m_textureName);
				}
			}
			materialCache.AddMaterial(material, materialInfo->GetId());
		}
	}
}


#if 1


Object *BuildPolyBox (TimeValue t) ;

void Import::LoadGeometries (dScene& scene, GeometryCache& meshCache, const MaterialCache& materialCache)
{
	dScene::Iterator iter (scene);
	for (iter.Begin(); iter; iter ++) {
		dScene::dTreeNode* const geometryNode = iter.GetNode();
		dNodeInfo* const info = scene.GetInfoFromNode(geometryNode);
		if (info->IsType(dGeometryNodeInfo::GetRttiType())) {
			if (info->GetTypeId() == dMeshNodeInfo::GetRttiType()) {

				// add the vertices
				PolyObject* const geometry = (PolyObject*) CreateInstance (GEOMOBJECT_CLASS_ID, Class_ID(POLYOBJ_CLASS_ID, 0));
//				PolyObject* const geometry = (PolyObject*) BuildPolyBox (TimeValue (0));

				meshCache.AddMesh(geometry, geometryNode);


				MNMesh& maxMesh = geometry->GetMesh();


//				float points[][3] = {{-20, 0, -20}, {20, 0, -20}, {20, 0, 20}, {-20, 0, 20}};				
//				// Do this just to make sure the mesh is clean to start with.
//				maxMesh.Clear();
//				// add all vertex as before
//				for (int j = 0; j < 4; j ++) {
//					Point3 vx (points[j][0], points[j][1], points[j][2]);
//					maxMesh.NewVert(vx);
//				}
//				// rather than create the edges manually, use the mesh to build it
//				// this basically creates the poly face
//				//maxMesh.NewQuad(0,1,2,3);
//				int f[] = {0, 1, 2, 3};
//				maxMesh.NewFace (0, 4, f, NULL, NULL);
//				// These are typically called to ensure integrity in the mesh.
//				maxMesh.InvalidateGeomCache();
//				maxMesh.InvalidateTopoCache();
//				// And this builds it based on the geometry setup previously.
//				maxMesh.FillInMesh();

				dMeshNodeInfo* const meshInfo = (dMeshNodeInfo*) scene.GetInfoFromNode(geometryNode);
				NewtonMesh* const mesh = meshInfo->GetMesh();

				//NewtonMeshTriangulate (mesh);

				int vertexCount = NewtonMeshGetVertexCount(mesh);
				int pointCount = NewtonMeshGetPointCount(mesh);

				// add all vertex
				int vertexStride = NewtonMeshGetVertexStrideInByte(mesh) / sizeof (dFloat64);
				dFloat64* const vertex = NewtonMeshGetVertexArray (mesh); 
				for (int j = 0; j < vertexCount; j ++) {
					Point3 vx (vertex[vertexStride * j + 0], vertex[vertexStride * j + 1], vertex[vertexStride * j + 2]);
					maxMesh.NewVert(vx);
				}

				// count the number of face and make a face map
				for (void* face = NewtonMeshGetFirstFace(mesh); face; face = NewtonMeshGetNextFace(mesh, face)) {
					if (!NewtonMeshIsFaceOpen(mesh, face)) {
						int faceIndices[256];
						int indexCount = NewtonMeshGetFaceIndexCount (mesh, face);
						NewtonMeshGetFaceIndices (mesh, face, faceIndices);
						maxMesh.NewFace (0, indexCount, faceIndices, NULL, NULL);
					}
				}

				maxMesh.InvalidateGeomCache();
				maxMesh.InvalidateTopoCache();
				// And this builds it based on the geometry setup previously.
				maxMesh.FillInMesh();

			} else {
				_ASSERTE (0);
			}
		}
	}
}


#else
void Import::LoadGeometries (dScene& scene, GeometryCache& meshCache, const MaterialCache& materialCache)
{
	dScene::Iterator iter (scene);
	for (iter.Begin(); iter; iter ++) {
		dScene::dTreeNode* const geometryNode = iter.GetNode();
		dNodeInfo* const info = scene.GetInfoFromNode(geometryNode);
		if (info->IsType(dGeometryNodeInfo::GetRttiType())) {
			if (info->GetTypeId() == dMeshNodeInfo::GetRttiType()) {

				// add the vertices
				//TriObject* const geometry = CreateNewTriObject();
				TriObject* const geometry = (TriObject*) CreateInstance (GEOMOBJECT_CLASS_ID, Class_ID(TRIOBJ_CLASS_ID, 0));
				meshCache.AddMesh(geometry, geometryNode);

				dMeshNodeInfo* const meshInfo = (dMeshNodeInfo*) scene.GetInfoFromNode(geometryNode);
				NewtonMesh* const mesh = meshInfo->GetMesh();

				NewtonMeshTriangulate (mesh);

				int vertexCount = NewtonMeshGetVertexCount(mesh);
				int pointCount = NewtonMeshGetPointCount(mesh);

				//int triangleCount = NewtonMeshGetTotalFaceCount(mesh);
				int triangleCount = 0;
				for (void* face = NewtonMeshGetFirstFace(mesh); face; face = NewtonMeshGetNextFace(mesh, face)) {
					if (!NewtonMeshIsFaceOpen(mesh, face)) {
						triangleCount += NewtonMeshGetFaceIndexCount (mesh, face) - 2;
					}
				}

				Mesh&  maxMesh = geometry->mesh;
				maxMesh.setNumVerts(vertexCount);
				maxMesh.setNumFaces(triangleCount);
				maxMesh.setNumTVerts(pointCount);
				maxMesh.setNumTVFaces(triangleCount);

				int vertexStride = NewtonMeshGetVertexStrideInByte(mesh) / sizeof (dFloat64);
				dFloat64* const vertex = NewtonMeshGetVertexArray (mesh); 
				for (int j = 0; j < vertexCount; j ++) {
					Point3 vx (vertex[vertexStride * j + 0], vertex[vertexStride * j + 1], vertex[vertexStride * j + 2]);
					maxMesh.setVert(j, vx);
				}

				int pointStride = NewtonMeshGetPointStrideInByte(mesh) / sizeof (dFloat64);
				dFloat64* const points = NewtonMeshGetUV0Array(mesh); 
				for (int j = 0; j < pointCount; j ++) {
					Point3 uv (dFloat(points[j * pointStride + 0]), dFloat(points[j * pointStride + 1]), 0.0f);
					maxMesh.setTVert(j, uv);
				}

				int faceIndex = 0;
				for (void* face = NewtonMeshGetFirstFace(mesh); face; face = NewtonMeshGetNextFace(mesh, face)) {
					if (!NewtonMeshIsFaceOpen(mesh, face)) {
						int vertexInices[256];
						int pointIndices[256];

						int indexCount = NewtonMeshGetFaceIndexCount (mesh, face);
						int matId = NewtonMeshGetFaceMaterial (mesh, face);

						MaterialProxi material;
						material.m_mtl = 0;
						material.m_matID = 0;
						MaterialCache::dTreeNode* const materialNode = materialCache.Find(matId);
						if (materialNode) {
							 material = materialNode->GetInfo();
						}
						
						NewtonMeshGetFaceIndices (mesh, face, vertexInices);
						NewtonMeshGetFacePointIndices (mesh, face, pointIndices);

						for (int i = 2; i < indexCount; i ++) {
							Face* f = &maxMesh.faces[faceIndex];
							TVFace* t = &maxMesh.tvFace[faceIndex];

							f->v[0] = vertexInices[0];
							f->v[1] = vertexInices[i - 1];
							f->v[2] = vertexInices[i];

							f->setEdgeVis(0, 1);
							f->setEdgeVis(1, 1);
							f->setEdgeVis(2, 1);
							f->setSmGroup(0);
							
							//f->setMatID((MtlID)matID);
							f->setMatID(material.m_matID);

							t->t[0] = pointIndices[0];
							t->t[1] = pointIndices[i - 1];
							t->t[2] = pointIndices[i];
							faceIndex ++;
						}
					}
				}

				SetSmoothingGroups (maxMesh);

		#if 0
				if (geom->m_uv1) {
					int texChannel = 2;

		//			maxMesh.setNumMaps (texChannel, TRUE);
					maxMesh.setMapSupport (texChannel);
					if (maxMesh.mapSupport(texChannel)) {
						maxMesh.setNumMapVerts (texChannel, triangleCount * 3);
						maxMesh.setNumMapFaces (texChannel, triangleCount);

						UVVert *tv = maxMesh.mapVerts(texChannel);
						faceIndex = 0;
						TVFace *tf = maxMesh.mapFaces(texChannel);
						for (segmentPtr = geom->GetFirst(); segmentPtr; segmentPtr = segmentPtr->GetNext()) {
							const dSubMesh& segment = segmentPtr->GetInfo();
							int triangleCount = segment.m_indexCount / 3;
							for (k = 0; k < triangleCount; k ++) {
								for (int m = 0; m < 3; m ++) {
									int index = segment.m_indexes[k * 3 + m];
									UVVert v (dFloat (geom->m_uv1[index * 2 + 0]), dFloat (geom->m_uv1[index * 2 + 1]), 0.0f);
									tv[faceIndex * 3 + m] = v;
									tf[faceIndex].t[m] = faceIndex * 3 + m;
								}
								faceIndex ++;
							}
						}
					}
				}
		#endif
			} else {
				_ASSERTE (0);
			}
		}
	}
}
#endif

INode* Import::CreateMaxHelperNode ()
{
	HelperObject* const obj = (HelperObject*)CreateInstance(HELPER_CLASS_ID, Class_ID(DUMMY_CLASS_ID, 0));
	INode* const maxNode = m_ip->CreateObjectNode(obj);

//maxNode->SetBoneNodeOnOff(TRUE, 0);
//maxNode->BoneAsLine(TRUE);
//maxNode->ShowBone(1);
	return maxNode;
}



//INode* Import::CreateMaxMeshNode (Mtl *mtl, const dMeshInstance* geometryInstance, const GeometryCache& meshCache)
INode* Import::CreateMaxMeshNode (dScene& scene, Mtl *mtl, dScene::dTreeNode* meshNode, const GeometryCache& meshCache)
{
//	dMesh* geometry;
//	geometry = geometryInstance->m_mesh;
	_ASSERTE (meshCache.Find(meshNode));
	TriObject* maxObj = (TriObject*) meshCache.Find(meshNode)->GetInfo();

//	maxObj->mesh.InvalidateTopologyCache();
//	maxObj->mesh.InvalidateGeomCache();
//	maxObj->mesh.buildNormals();

	INode* maxNode = m_ip->CreateObjectNode(maxObj);

	
	dMeshNodeInfo* meshInfo = (dMeshNodeInfo*) scene.GetInfoFromNode(meshNode);
	_ASSERTE (meshInfo->GetTypeId() == dMeshNodeInfo::GetRttiType());

	const dMatrix& matrix = meshInfo->GetPivotMatrix();
	Point3 posit (matrix.m_posit.m_x, matrix.m_posit.m_y, matrix.m_posit.m_z);
	dQuaternion quat (matrix);
	Quat rot (-quat.m_q1, -quat.m_q2, -quat.m_q3, quat.m_q0);
	maxNode->SetObjOffsetRot(rot);
	maxNode->SetObjOffsetPos(posit);

	maxNode->SetMtl(mtl);
	return maxNode;
}


void Import::LoadNode (dScene& scene, INode* maxParent, dScene::dTreeNode* node, const GeometryCache& meshCache, Mtl *mtl, MaxNodeChache& maxNodeCache)
{
	dScene::dTreeNode* geometryNode = NULL;
	for (void* ptr = scene.GetFirstChild(node); ptr; ptr = scene.GetNextChild(node, ptr) ) {
		dScene::dTreeNode* node = scene.GetNodeFromLink(ptr);
		dNodeInfo* info = scene.GetInfoFromNode(node);
		if (info->IsType(dGeometryNodeInfo::GetRttiType())) {
			geometryNode = node;
			break;
		}
	}


	INode* maxNode = NULL;
	if (geometryNode) {
//		maxNode = CreateMaxMeshNode (mtl, &geom[0]->GetInfo(), meshCache);
		maxNode = CreateMaxMeshNode (scene, mtl, geometryNode, meshCache);
		maxNodeCache.Insert (maxNode, geometryNode);
	} else {
		maxNode = CreateMaxHelperNode ();
	}
	
	_ASSERTE (maxNode);
	maxNodeCache.Insert (maxNode, node);

	if (maxParent) {
		maxParent->AttachChild(maxNode, 1);
	}

	dSceneNodeInfo* sceneInfo = (dSceneNodeInfo*)scene.GetInfoFromNode(node);

	TCHAR name[128];
	TCHAR tmp[128];
	strcpy (tmp, sceneInfo->GetName());
	for (int i = 0; tmp[i]; i ++) {
		if (isspace(tmp[i])) {
			tmp[i] = '_';
		}
	}
	strcpy (name, tmp);
	for (int i = 1; m_ip->GetINodeByName(name); i ++) {
		sprintf (name, "%s_%02d", tmp, i);
//		node->SetNameID(name);
	}
	maxNode->SetName(name);

	dMatrix transform (sceneInfo->GetTransform());
	dMatrix matrix;
	for (int i = 0; i < 4; i ++) {
		for (int j = 0; j < 4; j ++) {
			matrix[i][j] = transform[i][j];
		}
	}

	Matrix3 maxMatrix;
	maxMatrix.SetRow (0, *((Point3*) &matrix[0]));
	maxMatrix.SetRow (1, *((Point3*) &matrix[1]));
	maxMatrix.SetRow (2, *((Point3*) &matrix[2]));
	maxMatrix.SetRow (3, *((Point3*) &matrix[3]));
	maxNode->SetNodeTM(0, maxMatrix);

	for (void* ptr = scene.GetFirstChild(node); ptr; ptr = scene.GetNextChild(node, ptr) ) {
		dScene::dTreeNode* node = scene.GetNodeFromLink(ptr);
		dNodeInfo* info = scene.GetInfoFromNode(node);
		if (info->IsType(dSceneNodeInfo::GetRttiType())) {
			LoadNode (scene, maxNode, node, meshCache, mtl, maxNodeCache);
		}
	}
}


//void Import::LoadNode (INode* maxParent, const dMatrix& parentMatrix, dModel* model, dBone* node, const GeometryCache& meshCache, Mtl *mtl) 
void Import::LoadNodes (dScene& scene, const GeometryCache& meshCache, Mtl *mtl, MaxNodeChache& maxNodeCache)
{
	dScene::dTreeNode* const root = scene.GetRootNode();

	for (void* ptr = scene.GetFirstChild(root); ptr; ptr = scene.GetNextChild(root, ptr) ) {
		dScene::dTreeNode* node = scene.GetNodeFromLink(ptr);
		dNodeInfo* info = scene.GetInfoFromNode(node);
		if (info->IsType(dSceneNodeInfo::GetRttiType())) {
			LoadNode (scene, NULL, node, meshCache, mtl, maxNodeCache);
		}
	}
}


void Import::ApplyModifiers (dScene& scene, const MaxNodeChache& maxNodeCache)
{
	dScene::Iterator iter (scene);
	for (iter.Begin(); iter; iter ++) {
		dScene::dTreeNode* meshNode = iter.GetNode();
		dNodeInfo* info = scene.GetInfoFromNode(meshNode);
		if (info->IsType(dGeometryNodeInfo::GetRttiType())) {
			dScene::dTreeNode* skinModifierNode = NULL;	
			for (void* ptr = scene.GetFirstChild(meshNode); ptr; ptr = scene.GetNextChild(meshNode, ptr)) {
				dScene::dTreeNode* node = scene.GetNodeFromLink(ptr);
				dNodeInfo* info = scene.GetInfoFromNode(node);
				if (info->GetTypeId() == dGeometryNodeSkinModifierInfo::GetRttiType()) {
					skinModifierNode = node;
					break;
				}
			}

			if (skinModifierNode) {
				//create a skin modifier and add it
				Modifier* skinMod = (Modifier*) CreateInstance(OSM_CLASS_ID, SKIN_CLASSID);
				ISkinImportData* iskinImport = (ISkinImportData*) skinMod->GetInterface(I_SKINIMPORTDATA);
				INode* maxNode = maxNodeCache.Find(meshNode)->GetInfo(); 
				_ASSERTE (maxNode);

				IDerivedObject *derob = NULL;
				Object* obj = maxNode->GetObjectRef();	
				if(obj->SuperClassID() != GEN_DERIVOB_CLASS_ID)
				{
					derob = CreateDerivedObject(obj);
					maxNode->SetObjectRef(derob);
				} else {
					derob = (IDerivedObject*) obj;
				}
				derob->AddModifier(skinMod);

				dGeometryNodeSkinModifierInfo* skinModifier = (dGeometryNodeSkinModifierInfo*) scene.GetInfoFromNode(skinModifierNode);

				dMatrix matrix (skinModifier->m_shapeBindMatrix);
				Matrix3 bindPoseMatrix;
				bindPoseMatrix.SetRow (0, *((Point3*) &matrix[0]));
				bindPoseMatrix.SetRow (1, *((Point3*) &matrix[1]));
				bindPoseMatrix.SetRow (2, *((Point3*) &matrix[2]));
				bindPoseMatrix.SetRow (3, *((Point3*) &matrix[3]));
				iskinImport->SetSkinTm(maxNode, bindPoseMatrix, bindPoseMatrix);

				int maxNodeCount = 0;
				INode* maxNodes[1024];
			
				for (void* ptr = scene.GetFirstChild(skinModifierNode); ptr; ptr = scene.GetNextChild(skinModifierNode, ptr)) {
					dScene::dTreeNode* boneNode = scene.GetNodeFromLink(ptr);
					INode* skelBone = maxNodeCache.Find(boneNode)->GetInfo(); 
					maxNodes[maxNodeCount] = skelBone;
					maxNodeCount ++;
					skelBone->SetBoneNodeOnOff(TRUE, 0);
					skelBone->BoneAsLine(TRUE);
					skelBone->ShowBone(1);
					if (iskinImport->AddBoneEx(skelBone, TRUE)) {
						dSceneNodeInfo* sceneNode = (dSceneNodeInfo*) scene.GetInfoFromNode(boneNode);
						dMatrix matrix (sceneNode->GetTransform());
						Matrix3 bindPoseMatrix;
						bindPoseMatrix.SetRow (0, *((Point3*) &matrix[0]));
						bindPoseMatrix.SetRow (1, *((Point3*) &matrix[1]));
						bindPoseMatrix.SetRow (2, *((Point3*) &matrix[2]));
						bindPoseMatrix.SetRow (3, *((Point3*) &matrix[3]));
						iskinImport->SetBoneTm(skelBone, bindPoseMatrix, bindPoseMatrix);
					}
				}

				// must evaluate the node after adding bones
				maxNode->EvalWorldState(0);

				for (int i = 0; i < skinModifier->m_vertexCount; i ++) {
					Tab<float> weightList;
					Tab<INode*> boneNodeList;
					for (int j = 0; j < 4; j ++) {
						if (skinModifier->m_vertexWeights[i][j] > 1.0e-5f) {
							int boneIndex = skinModifier->m_boneWeightIndex[i].m_index[j];
							INode *skelBone = maxNodes[boneIndex];
							_ASSERTE (skelBone);		
							boneNodeList.Append (1, &skelBone);
							weightList.Append (1, &skinModifier->m_vertexWeights[i][j]);
						}
					}
					iskinImport->AddWeights(maxNode, i, boneNodeList, weightList);
				}
			}
		}
	}
}



void Import::SetSceneParameters()
{
	Interval range;

	range.SetStart(0);
	range.SetEnd(30);
	SetFrameRate(30);

	SetTicksPerFrame(160);

	range.SetStart(range.Start() * GetTicksPerFrame());
	range.SetEnd(range.End() * GetTicksPerFrame());

	m_ip->SetAnimRange(range);

	Point3 bkColor (0.0f, 0.0f, 0.0f);
	m_impip->SetBackGround(0, bkColor);

	Point3 amColor (0.3f, 0.3f, 0.3f);
	m_impip->SetAmbient(0, amColor);
}



Import::Import(const char* pathName, Interface* ip, ImpInterface* impip)
{
	// set the path for textures
	char* ptr = NULL;
	sprintf (m_path, "%s", pathName);
	for (int i = 0; m_path[i]; i ++) {
		if ((m_path[i] == '\\') || (m_path[i] == '/') ) {
			ptr = &m_path[i];
		}
	}
	*ptr = 0;


	m_ip = ip;
	m_impip = impip;
	m_succes = TRUE;
	MaterialCache materialCache (NewDefaultMultiMtl());

	SetSceneParameters();

	dFloat scale;
	scale = float (GetMasterScale(UNITS_METERS));
	dMatrix scaleMatrix (GetIdentityMatrix());
	scaleMatrix[0][0] = 1.0f / scale;
	scaleMatrix[1][1] = 1.0f / scale;
	scaleMatrix[2][2] = 1.0f / scale;
	dMatrix globalRotation (scaleMatrix * dPitchMatrix(3.14159265f * 0.5f) * dRollMatrix(3.14159265f * 0.5f));

	NewtonWorld* newton = NewtonCreate();
	dScene scene (newton);

	scene.Deserialize (pathName);
//	dScene::Iterator iter (scene);	
//	for (iter.Begin(); iter; iter ++) {
//		dScene::dTreeNode* node = iter.GetNode();
//		dNodeInfo* info = scene.GetInfoFromNode(node);
//		if (info->GetTypeId() == dMeshNodeInfo::GetRttiType()) {
//			dMeshNodeInfo* mesh = (dMeshNodeInfo*) scene.GetInfoFromNode(node);
//			mesh->ConvertToTriangles();
//		}
//	}
	scene.BakeTransform (globalRotation);
	
	GeometryCache geometryCache;
	MaxNodeChache maxNodeCache;

	LoadMaterials (scene, materialCache);
	LoadGeometries (scene, geometryCache, materialCache);
	LoadNodes (scene, geometryCache, materialCache.m_multiMat, maxNodeCache);
	ApplyModifiers (scene, maxNodeCache);

	scene.CleanUp();
	NewtonDestroy(newton);
}



Import::~Import(void)
{
}



void MakePQuad (MNFace *mf, int v1, int v2, int v3, int v4, DWORD smG, MtlID mt, int bias) 
{
	int vv[4];
	vv[0] = v1;
	vv[1+bias] = v2;
	vv[2] = v3;
	vv[3-bias] = v4;
	mf->MakePoly (4, vv);
	mf->smGroup = smG;
	mf->material = mt;
}


void MakeMapQuad (MNMapFace *mf, int v1, int v2, int v3, int v4, int bias) 
{
	int vv[4];
	vv[0] = v1;
	vv[1+bias] = v2;
	vv[2] = v3;
	vv[3-bias] = v4;
	mf->MakePoly (4, vv);
}


#define MAKE_BOTTOM_QUAD(v1,v2,v3,v4) MakePQuad(mm.F(nf),v1,v2,v3,v4,(1<<2),1,bias)
#define MAKE_MAP_QUAD(v1,v2,v3,v4) if(tf)MakeMapQuad(&(tf[nf]),v1,v2,v3,v4,bias);
#define MAKE_TOP_QUAD_FIX(v1,v2,v3,v4) MakePQuad(mm.F(nf),v1,v2,v3,v4,(1<<2),0,bias)
#define MAKE_BOTTOM_QUAD_FIX(v1,v2,v3,v4) MakePQuad(mm.F(nf),v1,v2,v3,v4,(1<<1),1,bias)


Object *BuildPolyBox (TimeValue t) 
{
	//	PolyObject *pobj = new PolyObject();
	PolyObject* const pobj = (PolyObject*) CreateInstance (GEOMOBJECT_CLASS_ID, Class_ID(POLYOBJ_CLASS_ID, 0));

	MNMesh & mm = pobj->mm;
	int wsegs = 1;
	int lsegs = 1;
	int hsegs = 1;
	float l = 40;
	float w = 40; 
	float h = 40;

	int genUVs = 0;
	int bias = 0;

	// Start the validity interval at forever and widdle it down.
	Interval gValid, tValid;
	tValid.SetInfinite();
	gValid = tValid;

//	pblock->GetValue(PB_LSEGS,t,lsegs,tValid);
//	pblock->GetValue(PB_WSEGS,t,wsegs,tValid);
//	pblock->GetValue(PB_HSEGS,t,hsegs,tValid);
//	pblock->GetValue(PB_GENUVS,t,genUVs,tValid);
//	LimitValue(lsegs, MIN_SEGMENTS, MAX_SEGMENTS);
//	LimitValue(wsegs, MIN_SEGMENTS, MAX_SEGMENTS);
//	LimitValue(hsegs, MIN_SEGMENTS, MAX_SEGMENTS);

	
//	pblock->GetValue(PB_LENGTH,t,l,gValid);
//	pblock->GetValue(PB_WIDTH,t,w,gValid);
//	pblock->GetValue(PB_HEIGHT,t,h,gValid);
//	if (h <0.0f ) bias = 2;

//	DWORD tParts = genUVs ? PART_TOPO|PART_TEXMAP : PART_TOPO;
//	DWORD otherStuff = OBJ_CHANNELS & ~(PART_GEOM|tParts);
//	pobj->SetPartValidity (otherStuff, FOREVER);
//	pobj->SetPartValidity (PART_GEOM, gValid);
//	pobj->SetPartValidity (tParts, tValid);

	// Number of verts
	// bottom : (lsegs+1)*(wsegs+1)
	// top    : (lsegs+1)*(wsegs+1)
	// sides  : (2*lsegs+2*wsegs)*(hsegs-1)

	// Number of rectangular faces.
	// bottom : (lsegs)*(wsegs)
	// top    : (lsegs)*(wsegs)
	// sides  : 2*(hsegs*lsegs)+2*(wsegs*lsegs)

	mm.Clear();

	int wsp1 = wsegs + 1;
	int nlayer = 2 * (lsegs + wsegs);
	int topStart = (lsegs+1)*(wsegs+1);
	int midStart = 2*topStart;

	int nverts = midStart+nlayer*(hsegs-1);
	int nfaces = 2*(lsegs*wsegs + hsegs*lsegs + wsegs*hsegs);

	mm.setNumVerts(nverts);
	mm.setNumFaces(nfaces);
	mm.InvalidateTopoCache();

	// Do mapping verts first, since they're easy.
//	int uvStart[6];
//	int ix, iy, iz;
	int nv;
	MNMapFace *tf = NULL;
	if (genUVs) {
/*
		int ls = lsegs+1;
		int ws = wsegs+1;
		int hs = hsegs+1;
		int ntverts = 2*(ls*hs + hs*ws + ws*ls);
		mm.SetMapNum (2);
		mm.M(1)->ClearFlag (MN_DEAD);
		mm.M(1)->setNumFaces (nfaces);
		mm.M(1)->setNumVerts (ntverts);
		UVVert *tv = mm.M(1)->v;
		tf = mm.M(1)->f;

		int xbase = 0;
		int ybase = ls*hs;
		int zbase = ls*hs + hs*ws;

		BOOL usePhysUVs = GetUsePhysicalScaleUVs();
		float maxW = usePhysUVs ? w : 1.0f;
		float maxL = usePhysUVs ? l : 1.0f;
		float maxH = usePhysUVs ? h : 1.0f;

		float dw = maxW/float(wsegs);
		float dl = maxL/float(lsegs);
		float dh = maxH/float(hsegs);

		if (w==0.0f) w = .0001f;
		if (l==0.0f) l = .0001f;
		if (h==0.0f) h = .0001f;
		float u,v;

		// Bottom of box.
		nv = 0;
		uvStart[0] = nv;
		v = 0.0f;
		for (iy =0; iy<ls; iy++) {
			u = 1.0f; 
			for (ix =0; ix<ws; ix++) {
				tv[nv] = UVVert (u, v, 0.0f);
				nv++; u-=dw;
			}
			v += dl;
		}

		// Top of box.
		uvStart[1] = nv;
		v = 0.0f;
		for (iy =0; iy<ls; iy++) {
			u = 0.0f; 
			for (ix =0; ix<ws; ix++) {
				tv[nv] = UVVert (u, v, 0.0f);
				nv++; u+=dw;
			}
			v += dl;
		}

		// Front Face
		uvStart[2] = nv;
		v = 0.0f; 
		for (iz =0; iz<hs; iz++) {
			u = 0.0f;
			for (ix =0; ix<ws; ix++) {
				tv[nv] = UVVert (u, v, 0.0f);
				nv++; u+=dw;
			}
			v += dh;
		}

		// Right Face
		uvStart[3] = nv;
		v = 0.0f;
		for (iz =0; iz<hs; iz++) {
			u = 0.0f; 
			for (iy =0; iy<ls; iy++) {
				tv[nv] = UVVert (u, v, 0.0f);
				nv++; u+=dl;
			}
			v += dh;
		}

		// Back Face
		uvStart[4] = nv;
		v = 0.0f; 
		for (iz =0; iz<hs; iz++) {
			u = 0.0f;
			for (ix =0; ix<ws; ix++) {
				tv[nv] = UVVert (u, v, 0.0f);
				nv++; u+=dw;
			}
			v += dh;
		}

		// Left Face
		uvStart[5] = nv;
		v = 0.0f;
		for (iz =0; iz<hs; iz++) {
			u = 0.0f; 
			for (iy =0; iy<ls; iy++) {
				tv[nv] = UVVert (u, v, 0.0f);
				nv++; u+=dl;
			}
			v += dh;
		}

		assert(nv==ntverts);
*/
	}

	nv = 0;

	Point3 vb(w/2.0f,l/2.0f,h);
	Point3 va(-vb.x, -vb.y, 0.0f);

	float dx = w/wsegs;
	float dy = l/lsegs;
	float dz = h/hsegs;

	// do bottom vertices.
	Point3 p;
	p.z = va.z;
	p.y = va.y;

	for (int iy=0; iy<=lsegs; iy++) {
		p.x = va.x;
		for (int ix=0; ix<=wsegs; ix++) {
			mm.P(nv) = p;
			nv++;
			p.x += dx;
		}
		p.y += dy;
	}


	int kv, nf = 0;

	// do bottom faces.
	// (Note that mapping verts are indexed the same as regular verts on the bottom.)
	for (int iy=0; iy<lsegs; iy++) {
		kv = iy*(wsegs+1);
		for (int ix=0; ix<wsegs; ix++) {
//			if (mPolyBoxSmoothingGroupFix) {
//				MAKE_BOTTOM_QUAD_FIX(kv,kv+wsegs+1,kv+wsegs+2,kv+1);
//			} else {
//				MAKE_BOTTOM_QUAD(kv,kv+wsegs+1,kv+wsegs+2,kv+1);
//			}

			MAKE_BOTTOM_QUAD(kv, kv + wsegs + 1, kv + wsegs + 2, kv+1);
			MAKE_MAP_QUAD (kv, kv + wsegs + 1, kv + wsegs + 2, kv + 1);
			nf++;
			kv++;
		}
	}
	assert(nf==lsegs*wsegs);
/*
	// do top vertices.
	// (Note that mapping verts are indexed the same as regular verts on the top.)
	p.z = vb.z;
	p.y = va.y;
	for(iy=0; iy<=lsegs; iy++) {
		p.x = va.x;
		for (ix=0; ix<=wsegs; ix++) {
			mm.P(nv) = p;
			p.x += dx;
			nv++;
		}
		p.y += dy;
	}

	// do top faces (lsegs*wsegs);
	for(iy=0; iy<lsegs; iy++) {
		kv = iy*(wsegs+1)+topStart;
		for (ix=0; ix<wsegs; ix++) {
			if (mPolyBoxSmoothingGroupFix) {
				MAKE_TOP_QUAD_FIX (kv,kv+1,kv+wsegs+2,kv+wsegs+1);
			} else {
				MAKE_TOP_QUAD (kv,kv+1,kv+wsegs+2,kv+wsegs+1);
			}
			MAKE_MAP_QUAD(kv,kv+1,kv+wsegs+2,kv+wsegs+1);
			nf++;
			kv++;
		}
	}
	assert(nf==lsegs*wsegs*2);

	// do middle vertices
	for(iz=1; iz<hsegs; iz++) {
		p.z = va.z + dz * iz;
		// front edge
		p.x = va.x;  p.y = va.y;
		for (ix=0; ix<wsegs; ix++) { mm.P(nv)=p; p.x += dx; nv++; }
		// right edge
		p.x = vb.x;	  p.y = va.y;
		for (iy=0; iy<lsegs; iy++) { mm.P(nv)=p; p.y += dy; nv++; }
		// back edge
		p.x =  vb.x;  p.y =  vb.y;
		for (ix=0; ix<wsegs; ix++) { mm.P(nv)=p; p.x -= dx; nv++; }
		// left edge
		p.x = va.x;  p.y =  vb.y;
		for (iy=0; iy<lsegs; iy++) { mm.P(nv)=p; p.y -= dy; nv++; }
	}

	int mv;
	if (hsegs==1) {
		// do FRONT faces -----------------------
		kv = 0;
		mv = topStart;
		for (ix=0; ix<wsegs; ix++) {
			MAKE_FRONT_QUAD (kv, kv+1, mv+1, mv);
			if (tf) {
				int mapv = uvStart[2]+ix;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+wsegs+2, mapv+wsegs+1);
			}
			nf++;
			kv++;
			mv++;
		}

		// do RIGHT faces.-----------------------
		kv = wsegs;  
		mv = topStart + kv;
		for (iy=0; iy<lsegs; iy++) {
			MAKE_RIGHT_QUAD(kv, kv+wsp1, mv+wsp1, mv);
			if (tf) {
				int mapv = uvStart[3]+iy;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+lsegs+2, mapv+lsegs+1);
			}
			nf++;
			kv += wsp1;
			mv += wsp1;
		}	

		// do BACK faces.-----------------------
		kv = topStart - 1;
		mv = midStart - 1;
		for (ix=0; ix<wsegs; ix++) {
			MAKE_BACK_QUAD(kv, kv-1, mv-1, mv);
			if (tf) {
				int mapv = uvStart[4]+ix;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+wsegs+2, mapv+wsegs+1);
			}
			nf++;
			kv --;
			mv --;
		}

		// do LEFT faces.----------------------
		kv = lsegs*(wsegs+1);  // index into bottom
		mv = topStart + kv;
		for (iy=0; iy<lsegs; iy++) {
			MAKE_LEFT_QUAD(kv, kv-wsp1, mv-wsp1, mv);
			if (tf) {
				int mapv = uvStart[5]+iy;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+lsegs+2, mapv+lsegs+1);
			}
			nf++;
			kv -= wsp1;
			mv -= wsp1;
		}
	} else {
		// do FRONT faces.
		kv = 0;
		mv = midStart;
		for(iz=0; iz<hsegs; iz++) {
			if (iz==hsegs-1) mv = topStart;
			for (ix=0; ix<wsegs; ix++) {
				MAKE_FRONT_QUAD(kv+ix, kv+ix+1, mv+ix+1, mv+ix);
				if (tf) {
					int mapv = uvStart[2] + iz*(wsegs+1) + ix;
					MAKE_MAP_QUAD (mapv, mapv+1, mapv+wsegs+2, mapv+wsegs+1);
				}
				nf++;
			}
			kv = mv;
			mv += nlayer;
		}

		assert(nf==lsegs*wsegs*2 + wsegs*hsegs);

		// do RIGHT faces.-------------------------
		// RIGHT bottom row:
		kv = wsegs; // into bottom layer. 
		mv = midStart + wsegs; // first layer of mid verts


		for (iy=0; iy<lsegs; iy++) {
			MAKE_RIGHT_QUAD(kv, kv+wsp1, mv+1, mv);
			if (tf) {
				int mapv = uvStart[3]+iy;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+lsegs+2, mapv+lsegs+1);
			}
			nf++;
			kv += wsp1;
			mv ++;
		}

		// RIGHT middle part:
		kv = midStart + wsegs; 
		for(iz=0; iz<hsegs-2; iz++) {
			mv = kv + nlayer;
			for (iy=0; iy<lsegs; iy++) {
				MAKE_RIGHT_QUAD(kv+iy, kv+iy+1, mv+iy+1, mv+iy);
				if (tf) {
					int mapv = uvStart[3] + (iz+1)*(lsegs+1) + iy;
					MAKE_MAP_QUAD (mapv, mapv+1, mapv+lsegs+2, mapv+lsegs+1);
				}
				nf++;
			}
			kv += nlayer;
		}

		// RIGHT top row:
		kv = midStart + wsegs + (hsegs-2)*nlayer; 
		mv = topStart + wsegs;
		for (iy=0; iy<lsegs; iy++) {
			MAKE_RIGHT_QUAD(kv, kv+1, mv+wsp1, mv);
			if (tf) {
				int mapv = uvStart[3] + (hsegs-1)*(lsegs+1) + iy;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+lsegs+2, mapv+lsegs+1);
			}
			nf++;
			mv += wsp1;
			kv++;
		}

		assert(nf==lsegs*wsegs*2 + wsegs*hsegs + lsegs*hsegs);

		// do BACK faces. ---------------------
		// BACK bottom row:
		kv = topStart - 1;
		mv = midStart + wsegs + lsegs;
		for (ix=0; ix<wsegs; ix++) {
			MAKE_BACK_QUAD(kv, kv-1, mv+1, mv);
			if (tf) {
				int mapv = uvStart[4]+ix;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+wsegs+2, mapv+wsegs+1);
			}
			nf++;
			kv --;
			mv ++;
		}

		// BACK middle part:
		kv = midStart + wsegs + lsegs; 
		for(iz=0; iz<hsegs-2; iz++) {
			mv = kv + nlayer;
			for (ix=0; ix<wsegs; ix++) {
				MAKE_BACK_QUAD(kv+ix, kv+ix+1, mv+ix+1, mv+ix);
				if (tf) {
					int mapv = uvStart[4] + (iz+1)*(wsegs+1) + ix;
					MAKE_MAP_QUAD (mapv, mapv+1, mapv+wsegs+2, mapv+wsegs+1);
				}
				nf++;
			}
			kv += nlayer;
		}

		// BACK top row:
		kv = midStart + wsegs + lsegs + (hsegs-2)*nlayer; 
		mv = topStart + lsegs*(wsegs+1)+wsegs;
		for (ix=0; ix<wsegs; ix++) {
			MAKE_BACK_QUAD(kv, kv+1, mv-1, mv);
			if (tf) {
				int mapv = uvStart[4] + (wsegs+1)*(hsegs-1) + ix;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+wsegs+2, mapv+wsegs+1);
			}
			nf++;
			mv --;
			kv ++;
		}

		assert(nf==lsegs*wsegs*2 + wsegs*hsegs*2 + lsegs*hsegs);

		// do LEFT faces. -----------------
		// LEFT bottom row:
		kv = lsegs*(wsegs+1);  // index into bottom
		mv = midStart + 2*wsegs +lsegs;
		for (iy=0; iy<lsegs; iy++) {
			int nextm = mv+1;
			if (iy==lsegs-1) nextm -= nlayer;
			MAKE_LEFT_QUAD(kv, kv-wsp1, nextm, mv);
			if (tf) {
				int mapv = uvStart[5]+iy;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+lsegs+2, mapv+lsegs+1);
			}
			nf++;
			kv -=wsp1;
			mv ++;
		}

		// LEFT middle part:
		kv = midStart + 2*wsegs + lsegs; 
		for(iz=0; iz<hsegs-2; iz++) {
			mv = kv + nlayer;
			for (iy=0; iy<lsegs; iy++) {
				int nextm = mv+1;
				int nextk = kv+iy+1;
				if (iy==lsegs-1) { 
					nextm -= nlayer;
					nextk -= nlayer;
				}
				MAKE_LEFT_QUAD(kv+iy, nextk, nextm, mv);
				if (tf) {
					int mapv = uvStart[5] + (iz+1)*(lsegs+1) + iy;
					MAKE_MAP_QUAD (mapv, mapv+1, mapv+lsegs+2, mapv+lsegs+1);
				}
				nf++;
				mv++;
			}
			kv += nlayer;
		}

		// LEFT top row:
		kv = midStart + 2*wsegs + lsegs+ (hsegs-2)*nlayer; 
		mv = topStart + lsegs*(wsegs+1);
		for (iy=0; iy<lsegs; iy++) {
			int nextk = kv+1;
			if (iy==lsegs-1) nextk -= nlayer;
			MAKE_LEFT_QUAD(kv, nextk, mv-wsp1, mv);
			if (tf) {
				int mapv = uvStart[5] + (hsegs-1)*(lsegs+1) + iy;
				MAKE_MAP_QUAD (mapv, mapv+1, mapv+lsegs+2, mapv+lsegs+1);
			}
			nf++;
			mv -= wsp1;
			kv++;
		}
	}

	//Make sure the MNMesh caches (both geometry and topology) are clean before returning the object
	mm.InvalidateGeomCache();
	mm.InvalidateTopoCache();
	mm.FillInMesh();
*/
	return (Object *) pobj;
}

