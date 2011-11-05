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
#include "../DemoEntityManager.h"
#include "../DemoCamera.h"
#include "../PhysicsUtils.h"



void BasicBoxStacks (DemoEntityManager* const scene)
{
	// suspend simulation before making changes to the physics world
	scene->StopsExecution ();

	// load the skybox
	scene->Append(new SkyBox());


	// load the scene from and alchemedia file format
	char fileName[2048];
	//GetWorkingFileName ("boxStacks_1.ngd", fileName);
	//GetWorkingFileName ("boxStacks_3.ngd", fileName);
	//GetWorkingFileName ("boxStacks.ngd", fileName);
	GetWorkingFileName ("pyramid40x40.ngd", fileName);
	scene->LoadScene (fileName);

	// place camera into position
	dQuaternion rot;
	dVector origin (-40.0f, 10.0f, 0.0f, 0.0f);
	scene->SetCameraMatrix(rot, origin);


	// resume the simulation
	scene->ContinueExecution();
}

