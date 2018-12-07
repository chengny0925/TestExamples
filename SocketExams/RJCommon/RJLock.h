#pragma once

#include "RJDefs.h"
#ifndef WIN
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#endif

class RJLock
{
public:
	RJLock();
	~RJLock();

	RJHRESULT Lock(RJDWORD  ulTimeout = RJ_INFINITE);
	RJHRESULT UnLock();

protected:
#ifdef WIN
	HANDLE m_hMutex;
#else
	pthread_mutex_t m_hMutex;
#endif
};


class RJAutoLock
{
public:
	RJAutoLock(RJLock& lock, RJDWORD  ulTimeout = RJ_INFINITE);
	~RJAutoLock();

protected:
	RJLock&  m_lock;
};

