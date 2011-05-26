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
#include "RigidBodyWorld.h"
#include "RigidBodyController.h"


class RigidBodyUndo: public RestoreObj 
{
	public:
	virtual void Restore(int undo);
	virtual void Redo();
	virtual int Size();
	virtual TSTR Description();

//	static void Hold(GeneralLight* light, int newVal, int oldVal)
//	{
//		if (theHold.Holding())
//			theHold.Put(new SimpleLightUndo<set, notify>(light, newVal, oldVal));
//	}

private:
//	RigidBodyUndo()

//	GeneralLight*		_light;
//	int					_redo;
//	int					_undo;
};




struct RendefGizmoInfo
{
	dMatrix m_matrix;
	GraphicsWindow* m_graphicWindow;
	RigidBodyController* m_me;
};



RigidBodyController::RigidBodyController(BOOL loading)
	:Control()
	,RigidBodyData()
	,m_scaleControl(NULL)
	,m_positionControl(NULL)
	,m_rotationControl(NULL)
	,pblock(NULL)
{
//	curval = Matrix3(1);
	blockUpdate = FALSE;

	if(!loading) {
		ReplaceReference(0, NewDefaultPositionController());
		ReplaceReference(1, NewDefaultRotationController());
		ReplaceReference(2, NewDefaultScaleController());
		ivalid = FOREVER;
	} else {
		ivalid.SetEmpty();
	}
}


RigidBodyController::~RigidBodyController()
{
	DeleteAllRefsFromMe();
}


IOResult RigidBodyController::Load(ILoad *iload)
{
	IOResult ret = Control::Load(iload);
	RigidBodyData* const data = this;

	iload->OpenChunk();
	data->Load(iload);
	iload->CloseChunk();

	return ret;
}

IOResult RigidBodyController::Save(ISave *isave)
{
	IOResult ret = Control::Save(isave);

	RigidBodyData* const data = this;
	Class_ID id (RigidBodyController::ClassID());

	isave->BeginChunk(USHORT (id.PartB()));
	data->Save(isave);
	isave->EndChunk();

	return ret;
}



void RigidBodyController::Init (const RigidBodyData& data, INode* const node)
{
	RigidBodyWorldDesc* const plugin = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();

	RigidBodyData* const myData = this;
	memcpy (myData, &data, sizeof (RigidBodyData));

	float scale = float (GetMasterScale(UNITS_METERS));
	ObjectState os (node->EvalWorldState(0)); 
	_ASSERTE (os.obj);

	Box3 box;
	os.obj->GetLocalBoundBox(0, node, NULL, box); 

	Point3 center (box.Center());
	Point3 boxSize (box.Width());

	dMatrix GeoMatrix (GetMatrixFromMaxMatrix (node->GetObjectTM(GetCOREInterface()->GetTime())));
	dMatrix nodeMatrix (GetMatrixFromMaxMatrix (node->GetNodeTM(GetCOREInterface()->GetTime())));

	dMatrix offset (GeoMatrix * nodeMatrix.Inverse4x4());

	offset.m_posit.m_x += center.x;
	offset.m_posit.m_y += center.y;
	offset.m_posit.m_z += center.z;
	offset = plugin->m_systemMatrixInv * offset * plugin->m_systemMatrix;

	offset.m_posit = offset.m_posit.Scale (scale);

	dVector size (plugin->m_systemMatrix.RotateVector(dVector(boxSize.x, boxSize.y, boxSize.z, 0.0f)));
	size = size.Scale (scale);

	NewtonCollision* collision = NULL;
	switch (m_collisionShape) 
	{
		case m_box:
		{
			collision = NewtonCreateBox(plugin->m_newton, size.m_x, size.m_y, size.m_z, 0, &offset[0][0]);
			break;
		}

		case m_convexHull:
		{
			_ASSERTE (0);
			break;
		}

		default:;
		{
			_ASSERTE (0);
		}
	}

	nodeMatrix = plugin->m_systemMatrixInv * nodeMatrix * plugin->m_systemMatrix;
	nodeMatrix.m_posit = nodeMatrix.m_posit.Scale (scale);

	m_body = NewtonCreateBody(plugin->m_newton, collision, &nodeMatrix[0][0]);
	NewtonBodySetUserData(m_body, node);

	NewtonConvexCollisionCalculateInertialMatrix (collision, &m_inertia[0], &m_origin[0]);	

	NewtonBodySetCentreOfMass(m_body, &m_origin[0]);
	NewtonBodySetMassMatrix(m_body, m_mass, m_mass * m_inertia.m_x, m_mass * m_inertia.m_y, m_mass * m_inertia.m_z);

	NewtonBodySetForceAndTorqueCallback(m_body, ApplyGravityForce);
	NewtonReleaseCollision(plugin->m_newton, collision);

}


void RigidBodyController::DeleteThis() 
{ 
	delete this; 
}		


// return number of ParamBlocks in this instance
int	RigidBodyController::NumParamBlocks() 
{ 
	return 1; 
}					

// return i'th ParamBlock
IParamBlock2* RigidBodyController::GetParamBlock(int i) 
{ 
	return pblock; 
} 

// return id'd ParamBlock
IParamBlock2* RigidBodyController::GetParamBlockByID(BlockID id) 
{ 
	return (pblock->ID() == id) ? pblock : NULL; 
} 


void RigidBodyController::GetClassName(TSTR& s) 
{
	s = GetString(IDS_CLASS_NAME);
}

Class_ID RigidBodyController::ClassID()
{
	return RigidBodyControllerDesc::GetDescriptor()->ClassID();
}


SClass_ID RigidBodyController::SuperClassID()
{
	return RigidBodyControllerDesc::GetDescriptor()->SuperClassID(); 
}

int RigidBodyController::NumSubs() 
{ 
	return 3; 
}

Animatable* RigidBodyController::SubAnim(int i)
{
	return GetReference(i);
}

TSTR RigidBodyController::SubAnimName(int i)
{
	switch (i) 
	{
	  case 0: return _T("POS");
	  case 1: return _T("ROT");
	  case 2: return _T("SCL");
	  default: return _T("");
	}
}

int RigidBodyController::NumRefs() 
{ 
	return 1; 
}

RefTargetHandle RigidBodyController::GetReference(int i)
{
	switch (i) 
	{
	  case 0: return m_positionControl;
	  case 1: return m_rotationControl;
	  case 2: return m_scaleControl;
	  default: return NULL;
	}
}

void RigidBodyController::SetReference(int i, RefTargetHandle rtarg)
{
	switch (i) 
	{
	  case 0: m_positionControl = (Control*)rtarg; break;
	  case 1: m_rotationControl = (Control*)rtarg; break;
	  case 2: m_scaleControl = (Control*)rtarg; break;
	}
}

RefResult RigidBodyController::NotifyRefChanged(Interval iv, RefTargetHandle hTarg, PartID& partID, RefMessage msg) 
{
	ivalid.SetEmpty();
	return REF_SUCCEED;
}

void RigidBodyController::PostCloneNode()
{
}


RefTargetHandle RigidBodyController::Clone(RemapDir& remap) 
{
	RigidBodyController* const myctrl = new RigidBodyController(); 

	myctrl->ReplaceReference(0, remap.CloneRef(m_positionControl));
	myctrl->ReplaceReference(1, remap.CloneRef(m_rotationControl));
	myctrl->ReplaceReference(2, remap.CloneRef(m_scaleControl));

	BaseClone(this, myctrl, remap);
	RigidBodyControllerDesc& controlDesc = *(RigidBodyControllerDesc*)RigidBodyControllerDesc::GetDescriptor();

	RigidBodyData* const srcData = this;
	RigidBodyData* const dstData = myctrl;

	memcpy (dstData, srcData, sizeof (RigidBodyData));
	dstData->m_body = NULL;

	return myctrl;
}

void RigidBodyController::Copy(Control* from)
{
	if (from->ClassID()==ClassID()) {
		RigidBodyController *ctrl = (RigidBodyController*)from;
		ReplaceReference(0, ctrl->m_positionControl);
		ReplaceReference(1, ctrl->m_rotationControl);
		ReplaceReference(2, ctrl->m_scaleControl);
//		curval = ctrl->curval;
		ivalid = ctrl->ivalid;
	} else {
		SetXFormPacket pckt;
		Interval iv;
		pckt.tmAxis.IdentityMatrix();			
		pckt.tmParent.IdentityMatrix();
		from->GetValue(0, &pckt.tmAxis, iv, CTRL_RELATIVE);			
		pckt.command = XFORM_SET;						 
		SetValue(0, &pckt, TRUE, CTRL_ABSOLUTE);	
	}
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
}

void RigidBodyController::Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& v, BOOL localOrigin, int commit)
{
	Point3 p = VectorTransform( tmAxis*Inverse(partm), v );
	m_positionControl->SetValue(t, &p, commit, CTRL_RELATIVE);	
} 

void RigidBodyController::SetAbsValue(TimeValue t, const Matrix3 &val, const Matrix3 &parent, int commit)
{
	Matrix3 pparent = ApplyInheritance(t, parent, m_positionControl);
	AffineParts parts;
	Matrix3 mat = val * Inverse(pparent);

	decomp_affine(mat, &parts);	

	m_positionControl->SetValue(t,&parts.t,commit,CTRL_ABSOLUTE);
	m_rotationControl->SetValue(t,&parts.q,commit,CTRL_ABSOLUTE);
	ScaleValue sv;
	sv.s = parts.k * parts.f;
	sv.q = parts.u;
	m_scaleControl->SetValue(t,&sv,commit,CTRL_ABSOLUTE);
}				



void RigidBodyController::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
{
	PreRefNotifyDependents();

	SetXFormPacket* ptr = (SetXFormPacket*)val;

	Matrix3 ptm = ApplyInheritance(t, ptr->tmParent, m_positionControl);	

	RigidBodyWorldDesc* const desc = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();
	float scale = float (GetMasterScale(UNITS_METERS));
	switch (ptr->command)
	{
		case XFORM_ROTATE:
		{
			_ASSERTE (0);
			break;
		}
		case XFORM_SCALE:
		{
			_ASSERTE (0);
			break;
		}
		case XFORM_MOVE:
		{
			Move(t, ptm, ptr->tmAxis, ptr->p, ptr->localOrigin, commit);
			break;
		}
		case XFORM_SET: 
		{
			SetAbsValue(t, ptr->tmAxis, ptm, commit);
			break;
		}
	}


	if (desc->m_updateRigidBodyMatrix) {
		Interval valid;
		Point3 posit;
		Quat rotation;

		//dMatrix matrix1 (GetMatrixFromMaxMatrix (ptr->tmAxis));

		m_positionControl->GetValue(t, &posit, valid, CTRL_ABSOLUTE);
		m_rotationControl->GetValue(t, &rotation, valid, CTRL_ABSOLUTE);
		dMatrix matrix (dQuaternion (rotation.w, rotation.x, rotation.y, rotation.z), dVector (posit.x, posit.y, posit.z, 1.0f));
		
		matrix = desc->m_systemMatrixInv * matrix * desc->m_systemMatrix;
		matrix.m_posit = matrix.m_posit.Scale (scale);

		// this does not handle scale yet, remember to extra scale for the matrix, 
		// the simplest way is removing the scale controller form the PRS controller
		NewtonBodySetMatrix (m_body, &matrix[0][0]);
	}

	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);

	PostRefNotifyDependents();
}


Matrix3 RigidBodyController::ApplyInheritance(TimeValue t, const Matrix3 &ptm, Control *pos, Point3 cpos, BOOL usecpos)
{
	return ptm;
}


void RigidBodyController::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	Matrix3& matrix = *((Matrix3*)val);
	matrix = ApplyInheritance(t, matrix, m_positionControl);	
	m_positionControl->GetValue(t, val, valid, method);
	m_rotationControl->GetValue(t, val, valid, method);
	m_scaleControl->GetValue(t, val, valid, method);
}


void RigidBodyController::ApplyGravityForce (const NewtonBody* const body, dFloat timestep, int threadIndex)
{
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);

	RigidBodyWorldDesc* const plugin = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();
	dVector gravity (plugin->m_gravity.Scale (mass));
	NewtonBodySetForce (body, &gravity.m_x);
}


void RigidBodyController::RenderGizmo (void* const userData, int vertexCount, const dFloat* const faceArray, int faceId)
{
	RendefGizmoInfo* const info = (RendefGizmoInfo*) userData;
	GraphicsWindow* const gw = info->m_graphicWindow;

	Point3 point3[256];
	info->m_matrix.TransformTriplex (&point3[0].x, sizeof (Point3), faceArray, 3 * sizeof (dFloat), vertexCount);
	gw->polyline (vertexCount, point3, NULL, NULL, TRUE, NULL);
}


int RigidBodyController::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags)
{
	if (!m_hideGizmos && m_body && NewtonBodyGetUserData(m_body)) {
		_ASSERTE (NewtonBodyGetUserData(m_body) == inode);
		GraphicsWindow* const gw = vpt->getGW();
		float scale = 1.0f / float (GetMasterScale(UNITS_METERS));
		

		RigidBodyWorldDesc* const desc = (RigidBodyWorldDesc*) RigidBodyWorldDesc::GetDescriptor();

		gw->setColor (LINE_COLOR, Point3 (1.0f, 0.5f, 0.0f));

		RendefGizmoInfo info;
		dMatrix scaleMtarix (GetIdentityMatrix());
		scaleMtarix[0][0] = scale;
		scaleMtarix[1][1] = scale;
		scaleMtarix[2][2] = scale;

		info.m_me = this;
		info.m_matrix = scaleMtarix * desc->m_systemMatrixInv;
		info.m_graphicWindow = gw;

		gw->setTransform(Matrix3(TRUE)); 
		NewtonCollision* const collision = NewtonBodyGetCollision(m_body);
		dMatrix matrix;
		NewtonBodyGetMatrix (m_body, &matrix[0][0]);
		NewtonCollisionForEachPolygonDo (collision, &matrix[0][0], RenderGizmo, &info);

		return 1;
	} else {
		return 0;
	}
}
