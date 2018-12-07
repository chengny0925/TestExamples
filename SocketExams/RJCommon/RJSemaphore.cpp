#include "RJSemaphore.h"



#include "RJSysUtil.h"

//#define TRACE  printf
#define TRACE


RJSemaphore::RJSemaphore(int nInitCount/* = 0*/)
{
	m_lWaiterCount = -nInitCount;
#ifdef _WIN32
	m_hSemaphore = CreateSemaphore(RJNULL, nInitCount, MAXLONG, RJNULL);
#else        
	m_lEventCount = nInitCount;
	//        m_hMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&m_hMutex, RJNULL);
	pthread_cond_init(&m_hEvent, RJNULL);
#endif
}

RJSemaphore::~RJSemaphore()
{
#ifdef _WIN32
	if (m_hSemaphore)
	{
		CloseHandle(m_hSemaphore);
	}
#else
	pthread_cond_destroy(&m_hEvent);
	pthread_mutex_destroy(&m_hMutex);
#endif
}

RJHRESULT RJSemaphore::ReleaseAll()
{
	RJHRESULT hr = RJ_E_FAIL;
#ifdef _WIN32
	RJAutoLock autoLock(m_lockCount);
	if (m_lWaiterCount > 0)
		hr = ReleaseSemaphore(m_hSemaphore, m_lWaiterCount, RJNULL) ? RJ_NOERROR : RJ_E_FAIL;
	else
		hr = RJ_NOERROR;
	m_lWaiterCount = 0;
#else
	pthread_mutex_lock(&m_hMutex);
	while (m_lEventCount < 0)
	{
		m_lEventCount++;
		if (m_lEventCount <= 0)
			pthread_cond_signal(&m_hEvent);

	}
	m_lWaiterCount = 0;
	pthread_mutex_unlock(&m_hMutex);
#endif

	return hr;
}

RJHRESULT RJSemaphore::Release(int lCount)
{
	RJHRESULT hr = RJ_E_FAIL;
#ifdef _WIN32
	RJAutoLock autoLock(m_lockCount);
	hr = ReleaseSemaphore(m_hSemaphore, lCount, RJNULL) ? RJ_NOERROR : RJ_E_FAIL;
	m_lWaiterCount -= lCount;
#else
	TRACE("enter release: m_lWaiterCount:%d, lCount:%d, m_lEventCount:%d\n", m_lWaiterCount, lCount, m_lEventCount);
	pthread_mutex_lock(&m_hMutex);
	m_lWaiterCount -= lCount;
	while (lCount > 0)
	{
		m_lEventCount++;
		if (m_lEventCount <= 0)
		{
			TRACE("enter pthread_cond_signal\n");
			int rc = pthread_cond_signal(&m_hEvent);
			TRACE("leave pthread_cond_signal, rc:%d\n", rc);
		}
		lCount--;
	}
	pthread_mutex_unlock(&m_hMutex);
	TRACE("Update: m_lWaiterCount:%d, lCount:%d, m_lEventCount:%d\n", m_lWaiterCount, lCount, m_lEventCount);
	TRACE("leave release\n");
#endif
	return RJ_NOERROR;
}

RJHRESULT RJSemaphore::Wait(RJDWORD dwTimeout)
{
#ifdef _WIN32
	if (m_hSemaphore == RJNULL) return RJ_E_FAIL;

	m_lockCount.Lock();
	m_lWaiterCount++;
	m_lockCount.UnLock();

	RJDWORD dwRes = WaitForSingleObject(m_hSemaphore, dwTimeout);

	if (WAIT_OBJECT_0 == dwRes)
		return RJ_NOERROR;
	else if (WAIT_TIMEOUT == dwRes)
		return RJ_E_TIMEOUT;
	else
		return RJ_E_FAIL;
#else
	TRACE("enter Wait\n");
	pthread_mutex_lock(&m_hMutex);

	m_lWaiterCount++;
	m_lEventCount--;
	TRACE("Update: m_lWaiterCount:%d, m_lEventCount:%d\n", m_lWaiterCount, m_lEventCount);
	if (m_lEventCount >= 0)
	{
		TRACE("leave Wait because of count\n");
		pthread_mutex_unlock(&m_hMutex);
		return RJ_NOERROR;
	}

	int rc;
	if (dwTimeout == RJ_INFINITE)
	{
		TRACE("enter pthread_cond_wait\n");
		rc = pthread_cond_wait(&m_hEvent, &m_hMutex);
		TRACE("leave pthread_cond_wait\n");
	}
	else
	{
		TRACE("enter pthread_cond_timedwait\n");


		struct timespec theTimeout = { 0, 0 };
		RJSysUtil::GetAbsTimeByRelTime(dwTimeout, &theTimeout);
		rc = pthread_cond_timedwait(&m_hEvent, &m_hMutex, &theTimeout);
		TRACE("leave pthread_cond_timedwait\n");
		if (rc == ETIMEDOUT)
		{
			m_lWaiterCount--;
			m_lEventCount++;
		}
	}
	pthread_mutex_unlock(&m_hMutex);
	TRACE("leave wait, rc:%d\n", rc);
	if (rc == 0) return RJ_S_OK;
	if (rc == ETIMEDOUT) return RJ_E_TIMEOUT;
	return RJ_E_FAIL;
#endif
}





