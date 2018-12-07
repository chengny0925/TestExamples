#include "RJRequestIdHelper.h"


RJRequestIdHelper::RJRequestIdHelper()
{
	m_requestId = 0;
}


RJRequestIdHelper::~RJRequestIdHelper()
{
}

int RJRequestIdHelper::GetNextRequestId()
{
	int reqId;
	m_lock.Lock();
	m_requestId++;
	reqId = m_requestId;
	m_lock.UnLock();

	return reqId;
}
