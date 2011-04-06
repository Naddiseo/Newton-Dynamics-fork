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


#ifndef __EXPORT_DESC_H__
#define __EXPORT_DESC_H__

#include "options.h"

class ExportAlchemediaDesc : public ClassDesc2 
{
public:
	int IsPublic();
	void* Create(BOOL loading = FALSE);
	const TCHAR* ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	const TCHAR* Category();
	const TCHAR* InternalName();
	HINSTANCE HInstance();
	static ClassDesc* GetControlDesc();
};


class AlchemediaMaxExport: public SceneExport 
{
	public:

	AlchemediaMaxExport();
	~AlchemediaMaxExport();		

	int ExtCount();					// Number of extensions supported
	const TCHAR* Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR* LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR* ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR* AuthorName();				// ASCII Author name
	const TCHAR* CopyrightMessage();			// ASCII Copyright message
	const TCHAR* OtherMessage1();			// Other message #1
	const TCHAR* OtherMessage2();			// Other message #2
	unsigned int Version();					// Version number * 100 (i.e. v3.01 = 301)
	void ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	int DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

	static BOOL CALLBACK ExportDescOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	static HWND hParams;
	Options m_options;
};



#endif

