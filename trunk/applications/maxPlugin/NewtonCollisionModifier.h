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


#ifndef __NEWTON_COLLISION_MODIFIER_H__
#define __NEWTON_COLLISION_MODIFIER_H__

#if 0

class NewtonCollisionModifierDesc: public ClassDesc2 
{
	public:
	NewtonCollisionModifierDesc ();
	int IsPublic();
	void* Create(BOOL loading = FALSE);
	const TCHAR* ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	const TCHAR* Category();
	const TCHAR* InternalName();
	HINSTANCE HInstance();
	void ResetClassParams (BOOL fileReset); 

	static ClassDesc* GetDescriptor();

};



class NewtonCollisionModifier: public Modifier 
{
	public:

	//Constructor/Destructor
	NewtonCollisionModifier();
	~NewtonCollisionModifier();		

	SClass_ID SuperClassID();
	Class_ID ClassID();


//	virtual Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat); 



	virtual void BeginEditParams (IObjParam *ip, ULONG flags, Animatable *prev);
	virtual void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
	virtual RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
	virtual CreateMouseCallBack* GetCreateMouseCallBack();
	virtual ChannelMask ChannelsUsed();
	virtual ChannelMask ChannelsChanged();
	virtual void ModifyObject(TimeValue t, ModContext &mc, ObjectState* os, INode *node);
	virtual Class_ID InputType();


	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
#endif

#endif

