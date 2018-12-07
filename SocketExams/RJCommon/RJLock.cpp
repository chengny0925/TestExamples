#include "RJLock.h"

#ifndef WIN
#include <time.h>
#include <sys/time.h>
#else
#endif


RJLock::RJLock()
{
#ifdef WIN	
	m_hMutex = CreateMutex(RJNULL, FALSE, RJNULL);
#else
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
#ifdef __APPLE__
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
#else
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
	pthread_mutex_init(&m_hMutex, &mattr);
	pthread_mutexattr_destroy(&mattr);
#endif
}


RJLock::~RJLock()
{
#ifdef WIN	
	if (m_hMutex)
	{
		CloseHandle(m_hMutex);
		m_hMutex = RJNULL;
	}
#else
	pthread_mutex_destroy(&m_hMutex);
#endif
}

RJHRESULT RJLock::Lock(RJDWORD ulTimeout)
{
#ifdef WIN	
	if (m_hMutex == RJNULL) return RJ_E_FAIL;

	RJDWORD dwRes = WaitForSingleObject(m_hMutex, ulTimeout);

	if (dwRes == WAIT_OBJECT_0)
	{
		return RJ_NOERROR;
	}
	else if (dwRes == WAIT_TIMEOUT)
	{
		return RJ_E_TIMEOUT;
	}

	return RJ_E_FAIL;
#else
	int rc = 0;
	if (ulTimeout == RJ_INFINITE)
	{
		rc = pthread_mutex_lock(&m_hMutex);
	}
	else
	{

		struct timespec theTimeout = { 0, 0 };

#ifdef __APPLE__

		struct timeval tv;

		gettimeofday(&tv, RJRJNULL);
		theTimeout.tv_sec = tv.tv_sec + (ulTimeout / 1000);
		theTimeout.tv_nsec = tv.tv_usec * 1000 + (ulTimeout % 1000) * 1000;
		//rc = pthread_mutex_timedlock(&m_hMutex, &theTimeout);
		//NEMOTODO:: for the timedlock

#else			
		//struct timespec theTimeout = {0, 0};
		rc = clock_gettime(CLOCK_REALTIME, &theTimeout);
		if (rc != 0) // If fail to get the real time
		{
			time_t tm;
			time(&tm);
			theTimeout.tv_sec = tm;
			theTimeout.tv_nsec = 0;
		}
		theTimeout.tv_sec += (ulTimeout / 1000);
		theTimeout.tv_nsec += (ulTimeout % 1000) * 1000;

		rc = pthread_mutex_timedlock(&m_hMutex, &theTimeout);
#endif			
	}

	if (rc == 0) return RJ_S_OK;
	if (rc == ETIMEDOUT || rc == EAGAIN) return RJ_E_TIMEOUT;
	return RJ_E_FAIL;
#endif
}

RJHRESULT RJLock::UnLock()
{
#ifdef WIN
	if (m_hMutex == RJNULL) return RJ_E_FAIL;

	return ReleaseMutex(m_hMutex) ? RJ_NOERROR : RJ_E_FAIL;
#else

	int rc = pthread_mutex_unlock(&m_hMutex);
	return (rc == 0 ? RJ_NOERROR : RJ_E_FAIL);
#endif
}

RJAutoLock::RJAutoLock(RJLock & lock, RJDWORD ulTimeout)
	: m_lock(lock)
{
	m_lock.Lock(ulTimeout);
}

RJAutoLock::~RJAutoLock()
{
	m_lock.UnLock();
}
