#pragma once
#include "mongoose.h"
#include <string>

using namespace std;

enum HttpError
{
	OK = 0,
	ConnectError = -1,
	TimeOut = -2
};

struct HttpRsp
{
	int error; // -1 connect error, -2 time out
	string rsp;
};

class HttpClient
{
public:
	HttpClient();
	~HttpClient();

	HttpRsp Get(string url, int timeOut = 3000 /*ms*/);

private:

	//static void ev_handler(struct mg_connection *c, int event_type, void *event_data, void *user_data);

	static void ev_handler(struct mg_connection *c, int event_type, void *event_data);




private:
	HttpRsp m_rsp;
	int m_exitFlag = 0;
};

