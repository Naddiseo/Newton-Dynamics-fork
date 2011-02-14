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

#ifndef __SDK_DEMOS_H__

class SDKDemos;
class DemoEntityManager;

typedef void (*LaunchSDKDemoCallback) (DemoEntityManager* scene);

class SDKDemos
{
	public:
	const char *m_name;
	const char *m_description;
	LaunchSDKDemoCallback m_launchDemoCallback;
};

void LoadDemo (DemoEntityManager* scene, int menuEntry);
void LoadMenuSeletions (HWND mainWindow);



#endif
