#include "RJTcpServer.h"
#include <iostream>
#include <string>
#include "uv.h"
#include "MyServer.h"

using namespace std;



int main()
{
	MyServer server;

	//cout << EAI_ADDRFAMILY << endl;
	//const char* buf;
	//int len;
	//uv_buf_t bufs[] = { uv_buf_t{ static_cast<size_t>(len), const_cast<char*>(buf) } };

	server.Init(12345);
	while (true)
	{
		char c;
		cin >> c;
		if (c == 'q')
		{
			break;
		}
		else if(c == 's')
		{
			server.Broadcast("server", 6);
		}
	}

	return 0;
}