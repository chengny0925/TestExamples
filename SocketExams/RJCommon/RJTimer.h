#pragma once
#include "RJDefs.h"
#include "RJSysUtil.h"

#define KT_NS_IN_MS  (RJUINT64)1000000

#define RJTimerVar(x) RJUINT64 x = RJSysUtil::GetNanoseconds()
#define RJTimerGet(x)  x = RJSysUtil::GetNanoseconds()
#define RJTimerGetNano() RJSysUtil::GetNanoseconds()
#define RJTimerDiff(beg, end) (end - beg)

class RJTimer
{
public:
	RJTimer();
	~RJTimer();

	RJUINT64 Begin();
	RJUINT64 End();
	RJUINT64 Diff();

private:
	RJUINT64 m_begin_nano;
	RJUINT64 m_end_nano;
};

