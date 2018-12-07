#include "RJTimer.h"
#include "RJSysUtil.h"


RJTimer::RJTimer()
{
}


RJTimer::~RJTimer()
{
}

RJUINT64 RJTimer::Begin()
{
	m_begin_nano = RJSysUtil::GetNanoseconds();
	return m_begin_nano;
}

RJUINT64 RJTimer::End()
{
	m_end_nano = RJSysUtil::GetNanoseconds();
	return m_end_nano;
}

RJUINT64 RJTimer::Diff()
{
	return m_end_nano - m_begin_nano;
}
