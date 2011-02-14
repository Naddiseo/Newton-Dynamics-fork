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


#ifndef __DG_WORKER_TREAD_H__
#define __DG_WORKER_TREAD_H__

#include "dgMemory.h"

//#define	DG_MAX_THREADS_HIVE_PARAMETERS		8 
//typedef void (*dgWorkerThreadTaskCallback) (void** const m_userParamArray, dgInt32 threadID);
typedef void (*dgWorkerThreadTaskCallback) (void* const context, dgInt32 threadID);


class dgWorkerThread
{
	class dgWorkerThreadJob
	{
		public:
		//void* m_userParamArray[DG_MAX_THREADS_HIVE_PARAMETERS];
		void* m_context;
		dgWorkerThreadTaskCallback m_callback;
	};

	public:
	DG_CLASS_ALLOCATOR(allocator)

	dgWorkerThread();
	~dgWorkerThread(void);

	void Init (dgMemoryAllocator* const allocator, dgInt32 id, dgUnsigned32* const jobQueueCompletedSignal);
	//void QueueJob (dgWorkerThreadTaskCallback callback, void** const userParamArray, dgInt32 paramCount);
	void QueueJob (dgWorkerThreadTaskCallback callback, void* const context);

	void ExecuteJobs ();
	bool IsThreadBusy () const {return m_jobsCount ? true : false;}
	void SetPerfomanceCounter(OnGetPerformanceCountCallback callback);

	private:
	void TaskExecuter();
	void RunAllMicroTask();
	dgInt32 GetNextJob(dgWorkerThreadJob& job);

	#if (defined (_WIN_32_VER) || defined (_WIN_64_VER) || defined (_MINGW_32_VER) || defined (_MINGW_64_VER))
		static unsigned _stdcall ThreadSystemCallback(void *param);
	#endif
	#if (defined (_LINUX_VER) || defined (_MAC_VER))
		static void* ThreadSystemCallback(void *Param);
	#endif


	dgWorkerThreadJob* m_queue;
	dgMemoryAllocator* m_allocator; 

	dgInt32 m_threadId;
	dgInt32 m_jobsCount;
	dgInt32 m_jobsCapacity;
	dgInt32 m_threaIsActive;
	dgInt32 m_contineExecution;
	dgUnsigned32 m_ticks;
	dgUnsigned32* m_jobQueueCompletedSignal;
	
	OnGetPerformanceCountCallback m_getPerformanceCount;	

#if (defined (_WIN_32_VER) || defined (_WIN_64_VER) || defined (_MINGW_32_VER) || defined (_MINGW_64_VER))
	HANDLE m_jobsInQueueSemaphore;
	uintptr_t m_threadhandle;
#endif

#if (defined (_LINUX_VER) || defined (_MAC_VER))
	pthread_t m_threadhandle;
#endif

	friend class dgThreadHive;
};

#endif
