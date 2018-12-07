#pragma once
#include "RJLock.h"

class RJRequestIdHelper
{
public:
	RJRequestIdHelper();
	~RJRequestIdHelper();

	int GetNextRequestId();

private:
	int m_requestId;
	RJLock m_lock;
};

