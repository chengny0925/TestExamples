#include "MyClient.h"

MyClient::MyClient()
{
}


MyClient::~MyClient()
{
}

int MyClient::Start(const char * ip, int port)
{
	m_client.m_callback = std::bind(&MyClient::func, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	int ret = m_client.Connect(ip, port);
	if (ret == 0)
	{
		m_is_running = true;
		m_pThread = new std::thread(&MyClient::MsgDespatcher, this);
	}

	return ret;
}

void MyClient::Stop()
{
	if (!m_is_running)
	{
		return;
	}

	m_is_running = false;
	m_client.Close();
}

void MyClient::SetClientCallBack(IClientCallBack * callBack)
{
	m_callBack = callBack;
}

int MyClient::SyncCallmethod()
{
	string msg = "test";
	SyncSendRtn result = SyncSend(SERVER_CMD_STX_COMMAND, msg.c_str(), strlen(msg.c_str()));
	if (result.errorCode != 0)
	{
		//error
		return -1;
	}
	string cmd = result.cmd;
	string value = result.value;
	cout << "return sync result, cmd=" << cmd << ", val=" << value << endl;
	if (strcmp(cmd.c_str(), SERVER_CMD_STX_COMMAND) == 0)
	{
		//parse value
	}
	else
	{
		//error
	}
	return 0;
}

AsyncSendRtn MyClient::AsyncSend(const char * cmd, const char * msg, int size)
{
	return Send(cmd, msg, size);
}

AsyncSendRtn MyClient::Send(const char * cmd, const char * msg, int size, int reqId)
{
	char* buf = new char[MSG_HEAD_LENGTH + size];
	if (reqId < 0)
	{
		reqId = m_requestIdHelper.GetNextRequestId();
	}
	string reqIdStr = std::to_string(reqId);
	strcpy(buf, reqIdStr.c_str());
	strcpy(buf + REQ_ID_LENGH, cmd);
	memcpy(buf + MSG_HEAD_LENGTH, msg, size);
	int ret = m_client.Send(buf, size + MSG_HEAD_LENGTH);
	delete buf;
	AsyncSendRtn result;
	result.errorCode = ret;
	result.reqId = reqId;
	return result;
}

void MyClient::func(int msg_type, const char* msg, int len)
{
	if (msg_type == ON_MSG)
	{
		OnMsg(msg, len);
		//cout << "ON_MSG: " << string(msg, len) << endl;
	}
	else if (msg_type == ON_CONNECTION)
	{
		OnConnection();
		cout << "ON_CONNECTION" << endl;
	}
	else if (msg_type == ON_ERROR)
	{
		OnError();
		cout << "ON_ERROR" << endl;
	}
	else if (msg_type == ON_DISCONNECTION)
	{
		OnError();
		cout << "ON_ERROR" << endl;
	}
}

void MyClient::OnMsg(const char * msg, int len)
{
	if (len <= MSG_HEAD_LENGTH)
	{
		return;
	}

	//if (strlen(msg.c_str()) > REQ_ID_LENGH)
	//{
	//	return;
	//}
	string reqIdStr = string(msg, REQ_ID_LENGH - 1);
	int id = atoi(reqIdStr.c_str());

	if (id != 0)
	{
		m_reqSemLock.Lock();
		RJSemaphore* sem = GetSyscReqSem(id);
		if (sem != nullptr)
		{
			m_reqResultMap[id] = string(msg + REQ_ID_LENGH, len - REQ_ID_LENGH);
			sem->Release();
			m_reqSemLock.UnLock();
			return;
		}
		m_reqSemLock.UnLock();
	}
	

	string buf;
	buf.resize(len);
	memcpy(&buf[0], msg, len);
	m_receiveMsgLock.Lock();
	m_receiveMsgVec.push_back(buf);
	m_receiveMsgLock.UnLock();
	m_receiveMsgSem.Release();
}

void MyClient::OnConnection()
{
	string buf("OnConnection");
	m_receiveMsgLock.Lock();
	m_receiveMsgVec.push_back(buf);
	m_receiveMsgLock.UnLock();
	m_receiveMsgSem.Release();
}

void MyClient::OnError()
{
	//callback
	if (m_callBack != nullptr)
	{
		m_callBack->OnError();
	}
}

void MyClient::OnDisConnection()
{
	if (m_callBack != nullptr)
	{
		m_callBack->OnDisConnection();
	}
}

void MyClient::MsgDespatcher()
{
	std::vector<string> temp_bufs;

	while (m_is_running)
	{
		m_receiveMsgSem.Wait(1);
		m_receiveMsgLock.Lock();
		{
			temp_bufs.swap(m_receiveMsgVec);
		}
		m_receiveMsgLock.UnLock();

		if (temp_bufs.size() > 0)
		{
			for (auto &iter : temp_bufs)
			{
				Handle(iter);
			}

			temp_bufs.clear();
		}


		//RJSysUtil::Sleep(1);
	}
}

void MyClient::Handle(string & msg)
{
	if (strcmp(msg.c_str(), "OnConnection") == 0)
	{
		//callback
		if (m_callBack != nullptr)
		{
			m_callBack->OnConnection();
		}
		return;
	}

	if (msg.size() <= MSG_HEAD_LENGTH)
	{
		return;
	}

	//if (strlen(msg.c_str()) > REQ_ID_LENGH)
	//{
	//	return;
	//}
	string reqIdStr = msg.substr(0, REQ_ID_LENGH);
	int id = atoi(reqIdStr.c_str());

	//if (strlen(msg.c_str() + REQ_ID_LENGH) > CMD_LENGTH)
	//{
	//	return;
	//}

	string cmd = msg.substr(REQ_ID_LENGH, CMD_LENGTH);

	string value = msg.substr(MSG_HEAD_LENGTH, msg.size() - MSG_HEAD_LENGTH);

	cout << "id=" << id << ", cmd=" << cmd << ", value=" << value << endl;

	//callback
	if (m_callBack != nullptr)
	{
		m_callBack->OnMsg(id, cmd, value);
	}
}

RJSemaphore* MyClient::GetSyscReqSem(int reqId)
{
	RJSemaphore* ret = nullptr;

	auto iter = m_reqSemMap.find(reqId);
	if (iter != m_reqSemMap.end())
	{
		ret = iter->second;
	}

	return ret;
}

SyncSendRtn MyClient::SyncSend(const char * cmd, const char * msg, int size, int ms)
{
	int reqId = m_requestIdHelper.GetNextRequestId();
	RJSemaphore sem;
	m_reqSemLock.Lock();
	m_reqSemMap.insert(make_pair(reqId, &sem));
	m_reqSemLock.UnLock();

	AsyncSendRtn rtn = Send(cmd, msg, size, reqId);
	SyncSendRtn result;
	if (rtn.errorCode != 0)
	{
		result.errorCode = rtn.errorCode;
	}
	else
	{
		RJHRESULT ret = sem.Wait(ms);
		result.errorCode = ret;
		if (ret == RJ_S_OK)
		{
			m_reqSemLock.Lock();
			auto iter = m_reqResultMap.find(reqId);
			if (iter != m_reqResultMap.end())
			{
				string& msg = iter->second;
				result.cmd = msg.substr(0, CMD_LENGTH);
				result.value = msg.substr(CMD_LENGTH, msg.size() - CMD_LENGTH);
				m_reqResultMap.erase(iter);
			}
			m_reqSemLock.UnLock();
		}
		else if (ret == RJ_E_TIMEOUT)
		{
			cout << "wait Semaphore timeout" << endl;
		}
		else if (ret == RJ_E_FAIL)
		{
			cout << "wait Semaphore fail" << endl;
		}
		else
		{
			cout << "wait Semaphore unkown error" << endl;
		}
	}

	m_reqSemLock.Lock();
	auto iter = m_reqSemMap.find(reqId);
	if (iter != m_reqSemMap.end())
	{
		m_reqSemMap.erase(iter);
	}

	auto iter2 = m_reqResultMap.find(reqId);
	if (iter2 != m_reqResultMap.end())
	{
		m_reqResultMap.erase(iter2);
	}

	m_reqSemLock.UnLock();


	return result;
}
