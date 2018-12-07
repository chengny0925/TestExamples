#pragma once
#include "RJDefs.h"

#ifdef WIN
#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 
#endif // !WIN32_LEAN_AND_MEAN 
#include "Windows.h"
#else
#include <pthread.h>
#endif

#ifdef WIN
#define SpinLockVar volatile UINT
#else
#define SpinLockVar pthread_spinlock_t
#endif 


class RJSpinLock
{
public:
	RJSpinLock();
	~RJSpinLock();

	RJHRESULT Lock();
	RJHRESULT UnLock();

protected:
#ifdef WIN
	volatile UINT m_spin;
#else
	pthread_spinlock_t m_spin;
#endif 
};

class RJSpinAutoLock
{
public:
	RJSpinAutoLock(RJSpinLock& lock);
	~RJSpinAutoLock();

protected:
	RJSpinLock&  m_lock;
};

class RJSpinLockHelper
{
public:
	~RJSpinLockHelper();

public:
	static RJSpinLockHelper* GetInstance();
	RJHRESULT Lock(SpinLockVar* spin);
	RJHRESULT UnLock(SpinLockVar* spin);
	void Init(SpinLockVar* spin);
	void UnInit(SpinLockVar* spin);

private:
	RJSpinLockHelper();

private:
	static RJSpinLockHelper* m_pInstance;
};


