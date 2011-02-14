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


#ifndef __DG_TREAD_HIVE_H__
#define __DG_TREAD_HIVE_H__


#include "dgWorkerThread.h"



class dgThreadHive  
{
	public:
	dgThreadHive(dgMemoryAllocator* const allocator);
	~dgThreadHive();

	void GetLock() const;
	void ReleaseLock() const;

	void GetIndirectLock(dgInt32* lockVar) const;
	void ReleaseIndirectLock(dgInt32* lockVar) const;

	dgInt32 GetThreadCount() const;
	dgInt32 GetMaxThreadCount() const;
	void SetThreadsCount (dgInt32 count);

	void SynchronizationBarrier ();
	void SyncThreads(dgInt32* const threads) const;
	void QueueJob (dgWorkerThreadTaskCallback callback, void* const context);

	void ClearTimers();
	void SetPerfomanceCounter(OnGetPerformanceCountCallback callback);
	dgUnsigned32 GetPerfomanceTicks (dgUnsigned32 threadIndex) const;

	private:
	void DestroydgThreads();

	dgInt32 m_numOfThreads;
	dgInt32 m_taskScheduler; 
	dgInt32 m_threadsStillWorking;
	mutable dgInt32 m_globalSpinLock;
	dgWorkerThread* m_workerBee;
	dgMemoryAllocator* m_allocator;
};



#endif
