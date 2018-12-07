#include "MyServer.h"
#include <RJSysUtil.h>


MyServer::MyServer()
{
	m_server = new RJTcpServer();
	m_server->m_callback = this;
	m_is_running = true;
	m_has_new_msg = false;
	m_pThread = new std::thread(&MyServer::MsgDespatcher, this);

}


MyServer::~MyServer()
{
}

void MyServer::OnMsg(uv_tcp_client * client, const char * msg, int size)
{
	ServerBufferStruct buff;
	buff.client = client;
	buff.msg.resize(size);
	memcpy(&buff.msg[0], msg, size);

	m_bufs_lock.lock();
	m_bufs.push_back(buff);
	m_has_new_msg = true;
	m_bufs_lock.unlock();

	/*string str = string(msg, size);
	string reqIdStr = string(msg, REQ_ID_LENGH);
	int id = atoi(reqIdStr.c_str());
	string cmd = string(msg + REQ_ID_LENGH, CMD_LENGTH);
	string value = string(msg + MSG_HEAD_LENGTH, size - MSG_HEAD_LENGTH);
	if (strcmp(cmd.c_str(), SERVER_CMD_STX_COMMAND) == 0)
	{
		EchoCmd(client, id, cmd, value);
	}

	cout << "id=" << id << ", cmd=" << cmd << ", value=" << value << endl;
	cout << "OnMsg " << client << ", " << str << endl;
	if (strcmp(str.c_str(), "test sysc call") == 0)
	{
		string msg = "sysc call result";
		cout << "send result to client, result=" << msg << endl;
		m_server->Send(&client->client, msg.c_str(), msg.size());
	}*/
}

void MyServer::EchoCmd(uv_tcp_client * client, int id, string & cmd, string & value)
{
	char* buf = new char[MSG_HEAD_LENGTH + value.size()];
	string reqIdStr = std::to_string(id);
	strcpy(buf, reqIdStr.c_str());
	strcpy(buf + REQ_ID_LENGH, cmd.c_str());
	memcpy(buf + MSG_HEAD_LENGTH, value.c_str(), value.size());
	m_server->Send(&client->client, buf, MSG_HEAD_LENGTH + value.size());
	delete buf;
}

void MyServer::OnNewConnection(uv_tcp_client * client)
{
	cout << "OnNewConnection " << client << endl;
	m_clientsLock.Lock();
	auto it = std::find(m_clients.begin(), m_clients.end(), client);
	if (it == m_clients.end())
	{
		m_clients.push_back(client);
	}
	m_clientsLock.UnLock();
}

void MyServer::OnDisconnection(uv_tcp_client * client)
{
	cout << "OnDisconnection " << client << endl;
	m_clientsLock.Lock();
	auto it = std::find(m_clients.begin(), m_clients.end(), client);
	if (it != m_clients.end())
	{
		m_clients.erase(it);
	}
	m_clientsLock.UnLock();
}

void MyServer::MsgDespatcher()
{
	std::vector<ServerBufferStruct> temp_bufs;

	while (m_is_running)
	{
		m_bufs_lock.lock();
		if (m_has_new_msg)
		{
			temp_bufs.swap(m_bufs);
			m_has_new_msg = false;
		}
		m_bufs_lock.unlock();

		if (temp_bufs.size() > 0)
		{
			for (auto &iter : temp_bufs)
			{
				DespatchMsg(iter);
			}

			temp_bufs.clear();
		}


		RJSysUtil::Sleep(1);
	}
}

void MyServer::DespatchMsg(ServerBufferStruct & msg)
{
	uv_tcp_client * client = msg.client;
	string& str = msg.msg;

	//parse msg, do some by cmd

	string reqIdStr = str.substr(0, REQ_ID_LENGH);
	int id = atoi(reqIdStr.c_str());
	string cmd = str.substr(REQ_ID_LENGH, CMD_LENGTH);
	string value = str.substr(MSG_HEAD_LENGTH, str.size() - MSG_HEAD_LENGTH);
	if (strcmp(cmd.c_str(), SERVER_CMD_STX_COMMAND) == 0)
	{
		EchoCmd(client, id, cmd, value);
	}

	cout << "id=" << id << ", cmd=" << cmd << ", value=" << value << endl;
	cout << "OnMsg " << client << ", " << str << endl;
	/*if (strcmp(str.c_str(), "test sysc call") == 0)
	{
		string msg = "sysc call result";
		cout << "send result to client, result=" << msg << endl;
		m_server->Send(&client->client, msg.c_str(), msg.size());
	}*/
}
