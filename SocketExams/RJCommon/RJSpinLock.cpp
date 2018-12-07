#include "RJSpinLock.h"


RJSpinLock::RJSpinLock()
{
#ifdef WIN
	m_spin = RJUNLOCK;
#else
	pthread_spin_init(&m_spin, PTHREAD_PROCESS_SHARED);
#endif 
}


RJSpinLock::~RJSpinLock()
{
#ifdef WIN
	m_spin = RJUNLOCK;
#else
	pthread_spin_destroy(&m_spin);
#endif 
}

RJHRESULT RJSpinLock::Lock()
{
	int res = 0;
#ifdef WIN
	while (InterlockedCompareExchange(&m_spin, RJLOCKED, RJUNLOCK) != RJUNLOCK)
	{
		Sleep(0);
	}
#else
	res = pthread_spin_lock(&m_spin);
#endif 
	return res;
}

RJHRESULT RJSpinLock::UnLock()
{
	int res = 0;
#ifdef WIN
	res = InterlockedExchange(&m_spin, RJUNLOCK);
#else
	res = pthread_spin_unlock(&m_spin);
#endif 
	return res;
}

RJSpinAutoLock::RJSpinAutoLock(RJSpinLock & lock)
	: m_lock(lock)
{
	m_lock.Lock();
}

RJSpinAutoLock::~RJSpinAutoLock()
{
	m_lock.UnLock();
}

RJSpinLockHelper* RJSpinLockHelper::m_pInstance = RJNULL;
RJSpinLockHelper::RJSpinLockHelper()
{
}

RJSpinLockHelper::~RJSpinLockHelper()
{
}

RJSpinLockHelper * RJSpinLockHelper::GetInstance()
{
	if (m_pInstance == RJNULL)
	{
		m_pInstance = new RJSpinLockHelper();
	}
	return m_pInstance;
}

RJHRESULT RJSpinLockHelper::Lock(SpinLockVar* spin)
{
	int res = 0;
#ifdef WIN
	while (InterlockedCompareExchange(spin, RJLOCKED, RJUNLOCK) != RJUNLOCK)
	{
		Sleep(0);
	}
#else
	res = pthread_spin_lock(spin);
#endif 
	return res;
}

RJHRESULT RJSpinLockHelper::UnLock(SpinLockVar* spin)
{
	int res = 0;
#ifdef WIN
	res = InterlockedExchange(spin, RJUNLOCK);
#else
	res = pthread_spin_unlock(spin);
#endif 
	return res;
}

void RJSpinLockHelper::Init(SpinLockVar * spin)
{
#ifdef WIN
	*spin = RJUNLOCK;
#else
	pthread_spin_init(spin, PTHREAD_PROCESS_SHARED);
#endif 
}

void RJSpinLockHelper::UnInit(SpinLockVar * spin)
{
#ifdef WIN
	*spin = RJUNLOCK;
#else
	pthread_spin_destroy(spin);
#endif 
}




