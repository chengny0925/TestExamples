#pragma once
#include "RJLock.h"
//#include <RJSpinLock.h>



class RJSemaphore
{
public:
	RJSemaphore(int nInitCount = 0);
	virtual ~RJSemaphore();

	RJHRESULT ReleaseAll();
	RJHRESULT Release(int lCount = 1);
	RJHRESULT Wait(RJDWORD dwTimeout = RJ_INFINITE);

private:

	int	m_lWaiterCount;

#ifdef WIN32
	RJLock m_lockCount;
	HANDLE	m_hSemaphore;
#else
	pthread_mutex_t	m_hMutex;
	pthread_cond_t	m_hEvent;
	int	m_lEventCount;
#endif
};


