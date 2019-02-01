#include "stdafx.h"
#include "WebsockThread.h"
#include <process.h>
#include  <log4cxx/log4cxx.h>
#include <log4cxx/logger.h>

LOGGER(logAppender, "WebsockThread");


WebsockThread::WebsockThread(ThreadPriority prior /*= Normal*/): thread_priority_(prior),
		thread_hanlde_(0), thread_id_(0), stopped_(false)
{

}

WebsockThread::~WebsockThread()
{
	if (thread_hanlde_)
		Join();
}

//typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);

bool WebsockThread::start()
{
	int prior = THREAD_PRIORITY_NORMAL;

	switch (thread_priority_)
	{
	case WebsockThread::TimeCritical:
		prior = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	case WebsockThread::Highest:
		prior = THREAD_PRIORITY_HIGHEST;
		break;
	case WebsockThread::AboveNormal:
		prior = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case WebsockThread::Normal:
		prior = THREAD_PRIORITY_NORMAL;
		break;
	case WebsockThread::BelowNormal:
		prior = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case WebsockThread::Lowest:
		prior = THREAD_PRIORITY_LOWEST;
		break;
	case WebsockThread::Idle:
		prior = THREAD_PRIORITY_IDLE;
		break;
	default:
		break;
	}

	thread_hanlde_ = (HANDLE)::_beginthreadex(NULL, 0, __entry, this, CREATE_SUSPENDED, &thread_id_);
	if (!thread_hanlde_)
		return false;

	BOOL ok = ::SetThreadPriority(thread_hanlde_, prior);
	if (!ok)
	{
		::CloseHandle(thread_hanlde_);
		thread_hanlde_ = 0;
		thread_id_ = 0;
		LOG4CXX_ERROR(logAppender, "SetThreadPriority" LOG_VAR(thread_priority_) );
		return false;
	}

	if (::ResumeThread(thread_hanlde_) == -1)
	{
		LOG4CXX_ERROR(logAppender, "SetThreadPriority ResumeThread()  " LOG_VAR(GetLastError()));
		::CloseHandle(thread_hanlde_);
		thread_hanlde_ = 0;
		thread_id_ = 0;
		return false;
	}

	return true;
}

void WebsockThread::Join()
{
	::WaitForSingleObject(thread_hanlde_, INFINITE);
	thread_hanlde_ = NULL;
}

void WebsockThread::Detach()
{
	::CloseHandle(thread_hanlde_);
	thread_hanlde_ = NULL;
}

void WebsockThread::Stop()
{
	stopped_ = true;
}

unsigned __stdcall WebsockThread::__entry(void *arg)
{
	WebsockThread *obj = (WebsockThread*)arg;
	if (obj)
		return obj->run();

	return -1;
}
