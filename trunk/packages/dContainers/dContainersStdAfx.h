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

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __D_CONTAINERS_STDAFX__
#define __D_CONTAINERS_STDAFX__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifdef _MSC_VER
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#endif
	#include <windows.h>
	#include <crtdbg.h>
#endif


#if (defined (_LINUX_VER) || defined (_MAC_VER))
#define _ASSERTE(x)
#endif


#ifdef _MAC_VER
	#ifndef _MAC_IPHONE
		#include <pthread.h>
		#include <semaphore.h>
	#endif	

	#include <unistd.h>
	#include <libkern/OSAtomic.h>
	#include <sys/sysctl.h>
#endif


#ifndef DTRACE
	#ifdef _DEBUG
		inline void dExpandTraceMessage (const char *fmt, ...)
		{
			#ifdef _MSC_VER 
				va_list v_args;
				char* const text = (char*) malloc (strlen (fmt) + 2048);

				text[0] = 0;
				va_start (v_args, fmt);     
				vsprintf(text, fmt, v_args);
				va_end (v_args);            

				OutputDebugStringA (text);

				free (text);
			#endif
		}

		#define DTRACE(x)										\
		{														\
			dExpandTraceMessage x;								\
		}
	#else
		#define DTRACE(x)
	#endif
#endif


#endif
