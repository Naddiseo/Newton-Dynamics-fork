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
#if 0

#include "NewtonCollisionModifier.h"


#define NEWTON_COLLISION_MODIFIER_ID Class_ID(0x475a60c2, 0x46e56d5a)


NewtonCollisionModifierDesc::NewtonCollisionModifierDesc ()
	:ClassDesc2()
{
}


int NewtonCollisionModifierDesc::IsPublic() 
{ 
	return TRUE; 
}

void* NewtonCollisionModifierDesc::Create(BOOL loading) 
{ 
//	return m_object;
	return new NewtonCollisionModifier();
}


const TCHAR* NewtonCollisionModifierDesc::ClassName() 
{ 
	return _T("Newton collision shape");
}

SClass_ID NewtonCollisionModifierDesc::SuperClassID() 
{ 
	return OSM_CLASS_ID; 
}

Class_ID NewtonCollisionModifierDesc::ClassID() 
{ 
	return NEWTON_COLLISION_MODIFIER_ID; 
}

const TCHAR* NewtonCollisionModifierDesc::Category() 
{ 
	return GetString(IDS_CATEGORY); 
}

const TCHAR* NewtonCollisionModifierDesc::InternalName() 
{ 
	return _T("CollisionModifier"); 
}	

HINSTANCE NewtonCollisionModifierDesc::HInstance() 
{ 
	return hInstance; 
}				

void NewtonCollisionModifierDesc::ResetClassParams (BOOL fileReset)
{
	ClassDesc2::ResetClassParams (fileReset);
}

ClassDesc* NewtonCollisionModifierDesc::GetDescriptor()
{ 
	static NewtonCollisionModifierDesc desc;
	return &desc; 
}



NewtonCollisionModifier::NewtonCollisionModifier()
	:Modifier ()
{
//	pblock2 = NULL;
//	NewtonCollisionModifierDesc::GetDescriptor()->MakeAutoParamBlocks(this);
//	_ASSERTE (pblock2);
}

NewtonCollisionModifier::~NewtonCollisionModifier()
{
}


SClass_ID NewtonCollisionModifier::SuperClassID()
{
	return NewtonCollisionModifierDesc::GetDescriptor()->SuperClassID(); 
}

Class_ID NewtonCollisionModifier::ClassID()
{
	return NewtonCollisionModifierDesc::GetDescriptor()->ClassID(); 
}



void NewtonCollisionModifier::BeginEditParams (IObjParam *ip, ULONG flags, Animatable *prev)
{
	_ASSERTE(0);
//	GetParmBlockDesc();
//	SimpleMod::BeginEditParams(ip, flags, prev);
//	NewtonCollisionModifierDesc::GetDescriptor()->BeginEditParams(ip, this, flags, prev);

//	m_worldPaneHandle = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_WORLD_PANE), NewtonWorldInfoPane::DialogProc, _T("Newton Alchemedia"), LPARAM (this));
//	m_rigidBodyPaneHandle = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_BODY_PANE), NewtonRigidBodyInfoPane::DialogProc, _T("RigidBodies properties"), LPARAM (this));

	ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_WORLD_PANE), DialogProc, _T("Newton Alchemedia"), LPARAM (this));
	ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_NEWTON_BODY_PANE), DialogProc, _T("RigidBodies properties"), LPARAM (this));

}

void NewtonCollisionModifier::EndEditParams(IObjParam *ip, ULONG flags, Animatable *next)
{
	_ASSERTE(0);
//	SimpleMod::EndEditParams(ip,flags,next);
//	NewtonCollisionModifierDesc::GetDescriptor()->EndEditParams(ip, this, flags, next);

//	ip->DeleteRollupPage(m_worldPaneHandle);
//	ip->DeleteRollupPage(m_rigidBodyPaneHandle);
}


RefResult NewtonCollisionModifier::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message) 
{ 
	_ASSERTE(0);
	return REF_SUCCEED; 
}


CreateMouseCallBack* NewtonCollisionModifier::GetCreateMouseCallBack()
{
	_ASSERTE(0);
	return NULL;
}

ChannelMask NewtonCollisionModifier::ChannelsUsed()
{
	return GEOM_CHANNEL;
}

ChannelMask NewtonCollisionModifier::ChannelsChanged()
{
	return GEOM_CHANNEL;
}

void NewtonCollisionModifier::ModifyObject(TimeValue t, ModContext &mc, ObjectState* os, INode *node)
{
//	_ASSERTE(0);
}

Class_ID NewtonCollisionModifier::InputType()
{
	return defObjectClassID;
}



INT_PTR CALLBACK NewtonCollisionModifier::DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

//	PhysicsPluginObject* const obj = (PhysicsPluginObject *)GetWindowLong (hWnd, GWL_USERDATA);

	switch (msg) 
	{
		case WM_INITDIALOG:
		{
//			PhysicsPluginObject* const obj = (PhysicsPluginObject *)lParam;
//			SetWindowLong(hWnd, GWL_USERDATA, (LONG)obj);

			//obj->NewtonRigidBodyInfoPane::Init(hWnd);
			break;
		}
/*
		case WM_DESTROY:
		{

			obj->NewtonRigidBodyInfoPane::Destroy(hWnd);
			break;
		}

		case WM_ENABLE:
		{
			EnableWindow(obj->m_rigidBodyPaneHandle, (BOOL) wParam);
			break;
		}

		case CC_SPINNER_CHANGE:
		{
			NewtonRigidBodyInfoPane& me = *obj;
			switch (LOWORD(wParam))
			{
			case IDC_RIGID_BODY_MASS_PINNER:
				{
					obj->SetSelectionMass (me.m_massEdit->GetFloat());
					break;
				}
			}
			return TRUE;
		}


		case WM_COMMAND:
		{
			NewtonRigidBodyInfoPane& me = *obj;
			switch (LOWORD(wParam)) 
			{
				case IDC_ATTACH_RIGIDBODY:
				{
					me.AttachSelectedRigiBodies();										
					break;
				}

				case IDC_DETACH_RIGIDBODY:
				{
					me.DetachSelectedRigiBodies();										
					break;
				}

				case IDC_SELECT_RIGIDBODIES:
				{
					me.SelectRigiBodies();										
					break;
				}

				case IDC_RIGID_BODY_COLLISION:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						HWND collisionShapeHandle = GetDlgItem(hWnd, IDC_RIGID_BODY_COLLISION);

						int index = CollisionShape(SendMessage(collisionShapeHandle, CB_GETCURSEL, 0, 0));  
						me.m_curCollsionShape = CollisionShape(SendMessage(collisionShapeHandle, CB_GETITEMDATA, index, 0));  
						for (dList<INode*>::dListNode* ptr = obj->m_currentSelection.GetFirst(); ptr; ptr = ptr->GetNext()) {
							INode* const node = ptr->GetInfo();
							AppDataChunk* const rigidBodyChunk = node->GetAppDataChunk(GetRigidBodyID(), node->SuperClassID(), 0); 
							if (rigidBodyChunk) {
								RidBodyData* const body = (RidBodyData*) rigidBodyChunk->data;
								body->m_collisionShape = me.m_curCollsionShape;
							}
						}

					}
					break;
				}

				case IDC_RIGID_BODY_MASS:
				{
					obj->SetSelectionMass (me.m_massEdit->GetFloat());
					break;
				}
			}
			break;
		}
*/
		default:
		return FALSE;
	}
	return TRUE;
}

#endif