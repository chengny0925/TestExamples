#pragma once
#include "RJDefs.h"
#include <string>
#include <vector>
#include <math.h>

using namespace std;

namespace RJSysUtil
{
	void ToString(int source, char* target);

	void ToString(long long int source, char* target);

	void ToString(unsigned int source, char* target);

	std::string FormatPath(const char* path);

	void Sleep(RJDWORD dwMS);

	std::string GetDate();

	std::string GetDate2();

	std::string GetDateTime();

	char GetFilePathSeparator();

	long GetFileSize(const char* file);

	RJBOOL IsDirExist(const char* path);

	RJBOOL IsFileExist(const char* path);

	RJHRESULT MakeDir(const char* path);

	RJHRESULT MakeDirs(const char* path);

#ifdef WIN
	std::string UTF8_To_GBK(const std::string& source);
	std::string GBK_To_UTF8(const std::string& source);
#else
	std::string GBK_To_UTF8(const std::string &source);
	std::string UTF8_To_GBK(const std::string &source);
#endif //WIN

	RJBOOL SplitString(char spCharacter, const string& objString, vector<string>& stringVector);

	void LogError(const char* msg);

	RJBOOL SplitIntString(char spCharacter, const string& objString, vector<int>& intVector);

	inline int RoundDouble(double number)
	{
		return (number > 0.0) ? floor(number + 0.5) : ceil(number - 0.5);
	}

	inline RJUINT64 CharArrayToInt(const char* instrumentId)
	{
		return *(RJUINT64 *)instrumentId;
	}

	inline RJUINT64 GetNanoseconds()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
	}

	unsigned StrStrip(char * s);

	unsigned StringStrip(string& str);

#ifndef _WIN32
	void GetAbsTimeByRelTime(WBXTFDWORD dwTime, struct timespec* pSpec);
#endif
}


