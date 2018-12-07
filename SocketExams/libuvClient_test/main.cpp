#include <RJTcpClient.h>
#include <iostream>
#include <string>
#include "uv.h"
#include "MyClient.h"
#include <stack>
using namespace std;

//RJClientCallBack

void func(int msg_type, const char* msg, int len)
{
	if (msg_type == ON_MSG)
	{
		cout << "ON_MSG: " << string(msg, len) << endl;
	}
	else if (msg_type == ON_CONNECTION)
	{
		cout << "ON_CONNECTION" << endl;
	}
	else if(msg_type == ON_ERROR)
	{
		cout << "ON_ERROR" << endl;
	}
}

int main()
{
	//RJTcpClient client;
	////cout << EAI_ADDRFAMILY << endl;
	//client.m_callback = std::bind(&func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	//client.Connect("127.0.0.1", 12345);
	//client.Connect("139.196.240.249", 12345);

	MyClient client;
	client.Start("127.0.0.1", 12345);

	while (true)
	{
		char c;
		c = getchar();
		string str;
		getline(cin, str);
		if (c == 'q')
		{
			break;
		}
		else if(c == 'i')
		{
			cout << "input string" << endl;

			getline(cin, str);
			client.AsyncSend(SERVER_CMD_STX_COMMAND, str.c_str(), str.length());
		}
		else if (c == 's')
		{
			client.AsyncSend(SERVER_CMD_STX_COMMAND, "sdsd", 4);
		}
		else if(c == 't')
		{
			/*string msg = "test sysc call";
			string s = client.SendSysc(msg.c_str(), msg.size());
			string cmd = s.substr(0, CMD_LENGTH);
			string value = s.substr(CMD_LENGTH, s.size() - CMD_LENGTH);
			cout << "return sysc result, cmd=" << cmd << ", val=" << value << endl;*/
			client.SyncCallmethod();
		}
	}

	return 0;
}