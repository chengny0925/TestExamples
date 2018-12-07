#include "RJSysUtil.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include "RJDefs.h"
#include "RJTimer.h"
#include <time.h>
#include <string.h>

#ifdef WIN
#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 
#endif // !WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <direct.h> 
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <signal.h>
#include <iconv.h>
#endif

#define PathMaxLength 1024
#define ERROR_LOG_FILENAME "error.log"


void RJSysUtil::ToString(int source, char * target)
{
	int temp = source;
	int length = 0;
	int index = 0;

	if (source < 0)
	{
		length++;
		index++;
		target[0] = '-';
		temp = -temp;
	}

	while (temp > 0)
	{
		temp /= 10;
		length++;
	}

	temp = source < 0 ? -source : source;
	target[length] = '\0';
	while (length > index)
	{
		target[length - 1] = temp % 10 + '0';
		temp /= 10;
		length--;
	}
}

void RJSysUtil::ToString(long long int source, char * target)
{
	int temp = source;
	int length = 0;
	int index = 0;

	if (source < 0)
	{
		length++;
		index++;
		target[0] = '-';
		temp = -temp;
	}

	while (temp > 0)
	{
		temp /= 10;
		length++;
	}

	temp = source < 0 ? -source : source;
	target[length] = '\0';
	while (length > index)
	{
		target[length - 1] = temp % 10 + '0';
		temp /= 10;
		length--;
	}
}

void RJSysUtil::ToString(unsigned int source, char * target)
{
	unsigned int temp = source;
	int length = 0;

	while (temp > 0)
	{
		temp /= 10;
		length++;
	}

	temp = source;
	target[length] = '\0';
	while (length > 0)
	{
		target[length - 1] = temp % 10 + '0';
		temp /= 10;
		length--;
	}
}

std::string RJSysUtil::FormatPath(const char * path)
{
	std::string sPath(path);
	std::string::size_type i = -1;
	std::string::size_type npos = -1;
	while (npos != (i = sPath.find("\\")))
	{
		sPath.replace(i, 1, "/");
	}
	if (sPath.size() > 0)
	{
		if (sPath[sPath.size() - 1] == '/' || sPath[sPath.size() - 1] == '\\')sPath.erase(sPath.size() - 1);
	}
	return sPath;
}

void RJSysUtil::Sleep(RJDWORD dwMS)
{
#ifdef WIN
	::Sleep(dwMS);
#else
	usleep(dwMS * 1000);
#endif
}

std::string RJSysUtil::GetDate()
{
	time_t timer;
	tm *tblock;
	timer = time(NULL);
	tblock = localtime(&timer);

	char buf[11];
	sprintf(buf, "%04d-%02d-%02d", tblock->tm_year + 1900, tblock->tm_mon + 1, tblock->tm_mday);
	return buf;
}

std::string RJSysUtil::GetDate2()
{
	time_t timer;
	tm *tblock;
	timer = time(NULL);
	tblock = localtime(&timer);

	char buf[9];
	sprintf(buf, "%04d%02d%02d", tblock->tm_year + 1900, tblock->tm_mon + 1, tblock->tm_mday);
	return buf;
}



std::string RJSysUtil::GetDateTime()
{
	time_t timer;
	tm *tblock;
	timer = time(NULL);
	tblock = localtime(&timer);

	char buf[16];
	sprintf(buf, "%04d%02d%02d_%02d%02d%02d", tblock->tm_year + 1900, tblock->tm_mon + 1, tblock->tm_mday,
		tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
	return buf;
}

char RJSysUtil::GetFilePathSeparator()
{
#ifdef WIN
	return '\\';
#else
	return '/';
#endif // WIN

}

RJBOOL RJSysUtil::IsDirExist(const char * path)
{
	RJBOOL bExist = RJTRUE;
#ifdef WIN

	RJDWORD res = GetFileAttributes(path);
	if (res == -1)
	{
		bExist = RJFALSE;
	}
	if (FILE_ATTRIBUTE_DIRECTORY != (res&FILE_ATTRIBUTE_DIRECTORY))
	{
		bExist = RJTRUE;
}

#else
	int ret = access(path, 0);
	if (ret == -1)
	{
		bExist = RJFALSE;
	}
#endif 

	return bExist;
}

RJBOOL RJSysUtil::IsFileExist(const char* path)
{
	bool exist = RJFALSE;
	std::ifstream in(path);
	if (in)
	{
		exist = RJTRUE;
	}

	return exist;
}

RJHRESULT RJSysUtil::MakeDir(const char * path)
{
	RJHRESULT ret = RJ_S_OK;
#ifdef WIN
	int flag = _mkdir(path);
#else
	int flag = mkdir(path, 0777);
#endif 

	if (flag == -1)
	{
		ret = RJ_S_FALSE;
	}
	return ret;
}

RJHRESULT RJSysUtil::MakeDirs(const char * path)
{
	RJHRESULT ret = RJ_S_OK;
	if (strlen(path) == 0 || path == NULL) {
		return ret;
	}
	char tmp[PathMaxLength];
	Strcpy(tmp, path);
	char *p = strrchr(tmp, GetFilePathSeparator());
	if (p)
	{
		*p = '\0';
		ret = MakeDirs(tmp);
		if (ret == RJ_S_OK && !IsDirExist(path))
		{
			ret = MakeDir(path);
		}
	}
	else
	{
		if (!IsDirExist(path))
		{
			ret = MakeDir(path);
		}
	}

	return ret;
}

#ifdef WIN
std::string RJSysUtil::UTF8_To_GBK(const std::string & source)
{
	enum { GB2312 = 936 };

	unsigned long len = ::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, NULL, NULL);
	if (len == 0)
		return std::string();
	wchar_t *wide_char_buffer = new wchar_t[len];
	::MultiByteToWideChar(CP_UTF8, NULL, source.c_str(), -1, wide_char_buffer, len);

	len = ::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);
	if (len == 0)
	{
		delete[] wide_char_buffer;
		return std::string();
	}
	char *multi_byte_buffer = new char[len];
	::WideCharToMultiByte(GB2312, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);

	std::string dest(multi_byte_buffer);
	delete[] wide_char_buffer;
	delete[] multi_byte_buffer;
	return dest;
}

std::string RJSysUtil::GBK_To_UTF8(const std::string &source)
{
	enum { GB2312 = 936 };

	unsigned long len = ::MultiByteToWideChar(GB2312, NULL, source.c_str(), -1, NULL, NULL);
	if (len == 0)
		return std::string();
	wchar_t *wide_char_buffer = new wchar_t[len];
	::MultiByteToWideChar(GB2312, NULL, source.c_str(), -1, wide_char_buffer, len);

	len = ::WideCharToMultiByte(CP_UTF8, NULL, wide_char_buffer, -1, NULL, NULL, NULL, NULL);
	if (len == 0)
	{
		delete[] wide_char_buffer;
		return std::string();
	}
	char *multi_byte_buffer = new char[len];
	::WideCharToMultiByte(CP_UTF8, NULL, wide_char_buffer, -1, multi_byte_buffer, len, NULL, NULL);

	std::string dest(multi_byte_buffer);
	delete[] wide_char_buffer;
	delete[] multi_byte_buffer;
	return dest;
}
#else

int code_convert(const char *from_charset, const char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
	return 0;
}

std::string RJSysUtil::GBK_To_UTF8(const std::string &source)
{
	char temp[512];
	code_convert("gb2312", "utf-8", (char *)source.c_str(), source.size(), temp, 512);
	return temp;
}

std::string RJSysUtil::UTF8_To_GBK(const std::string &source)
{
	char temp[512];
	code_convert("utf-8", "gb2312", (char *)source.c_str(), source.size(), temp, 512);
	return temp;
}
#endif // WIN

RJBOOL RJSysUtil::SplitIntString(char spCharacter, const string& objString, vector<int>& intVector)
{
	vector<string> strVector;
	SplitString(spCharacter, objString, strVector);
	vector<string>::iterator iter = strVector.begin();
	while (iter != strVector.end())
	{
		intVector.push_back(atoi(iter->c_str()));
		iter++;
	}
	return RJTRUE;
}

RJBOOL RJSysUtil::SplitString(char spCharacter, const string& objString, vector<string>& stringVector)
{
	if (objString.length() == 0)
	{
		return RJTRUE;
	}

	size_t posBegin = 0;
	size_t posEnd = 0;
	bool lastObjStore = true;

	while (posEnd != string::npos)
	{
		posBegin = posEnd;
		posEnd = objString.find(spCharacter, posBegin);

		if (posBegin == posEnd)
		{
			posEnd += 1;
			continue;
		}

		if (posEnd == string::npos)
		{
			stringVector.push_back(objString.substr(posBegin, objString.length() - posBegin));
			break;
		}

		stringVector.push_back(objString.substr(posBegin, posEnd - posBegin));
		posEnd += 1;
	}
	return RJTRUE;
}

void RJSysUtil::LogError(const char* msg)
{
	ofstream outfile;
	outfile.open(ERROR_LOG_FILENAME, ios::app);
	if (!outfile) //检查文件是否正常打开//不是用于检查文件是否存在
	{
		STDOUT(std::cout << "can't open error log file " << ERROR_LOG_FILENAME << endl);
	}
	else
	{
		outfile << msg << "," << RJTimerGetNano() << endl;
		outfile.close();
	}
}

long RJSysUtil::GetFileSize(const char * file)
{
	std::ifstream in(file);
	if (!in.is_open())
		return 0;

	in.seekg(0, std::ios_base::end);
	std::streampos sp = in.tellg();
	in.close();
	return sp;
}

unsigned RJSysUtil::StrStrip(char * s)
{
	char *last = NULL;
	char *dest = s;

	if (s == NULL) return 0;

	last = s + strlen(s);
	while (isspace((int)*s) && *s) s++;
	while (last > s) {
		if (!isspace((int)*(last - 1)))
			break;
		last--;
	}
	*last = (char)0;

	memmove(dest, s, last - s + 1);
	return last - s;
}

unsigned RJSysUtil::StringStrip(string& str)
{
	unsigned length = str.size();
	if (length > 0)
	{
		char* temp = new char[length + 1];
		memcpy(temp, str.c_str(), length + 1);
		StrStrip(temp);
		str = temp;
		delete[] temp;
	}

	return str.size();
}

#ifndef _WIN32

void RJSysUtil::GetAbsTimeByRelTime(WBXTFDWORD ulTimeout, struct timespec* pSpec)
{
	if (pSpec == WBXTFNULL) return;
#ifdef __APPLE__
	struct timeval tv;
	gettimeofday(&tv, WBXTFNULL);
	pSpec->tv_sec = tv.tv_sec;
	pSpec->tv_nsec = tv.tv_usec * 1000;
#else	

	int rc = clock_gettime(CLOCK_REALTIME, pSpec);
	if (rc != 0) // If fail to get the real time
	{
		time_t tm;
		time(&tm);
		pSpec->tv_sec = tm;
		pSpec->tv_nsec = 0;
	}
#endif
	pSpec->tv_sec += (ulTimeout / 1000);
	pSpec->tv_nsec += (ulTimeout % 1000) * MS_2_NS;
	if (pSpec->tv_nsec >= SECOND_2_NS)
	{
		pSpec->tv_sec += 1;
		pSpec->tv_nsec -= SECOND_2_NS;
	}
}

#endif