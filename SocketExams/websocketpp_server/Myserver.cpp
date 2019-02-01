#include "Myserver.h"

Myserver::Myserver() : m_pServer(nullptr), m_pThread(nullptr), m_running(false), m_con_id(0)
{
}

Myserver::~Myserver()
{
	Stop();
}

int Myserver::Init(int port)
{
	m_pServer = new WebsocketServer();
	m_pServer->Init(port);
	m_pServer->SetCallBack(this);
	return 0;
}

int Myserver::Start()
{
	m_running = true;
	m_pThread = new std::thread(bind(&Myserver::process_messages, this));
	int ret = m_pServer->Start();

	return ret;
}

void Myserver::Stop()
{
	if (m_pServer != nullptr)
	{
		delete m_pServer;
		m_pServer = nullptr;
	}

	if (m_pThread != nullptr)
	{
		m_running = false;
		m_pThread->join();
		delete m_pThread;
		m_pThread = nullptr;
	}
}

void Myserver::NewConnection(WSHandle wsi)
{
}

void Myserver::Disconnection(WSHandle wsi)
{
	lock_guard<mutex> guard(m_connection_lock);
	m_connections.erase(wsi);
	m_con_map.erase(wsi);
}

void Myserver::TextReceived(WSHandle wsi, const char * message, int len)
{
	{
		lock_guard<mutex> guard(m_action_lock);
		m_receiveMsgVec.push(ConnectionMsg(wsi, std::string(message, len)));
	}
	m_action_cond.notify_one();
}

void Myserver::BinaryReceived(WSHandle wsi, const char * message, int len)
{
}

void Myserver::process_messages()
{
	std::chrono::seconds wait_time = std::chrono::seconds(1);
	while (m_running)
	{
		unique_lock<mutex> lock(m_action_lock);

		m_action_cond.wait_for(lock, wait_time);

		if (!m_receiveMsgVec.empty())
		{
			ConnectionMsg msg = m_receiveMsgVec.front();
			m_receiveMsgVec.pop();
			lock.unlock();

			handle_msg(msg);
		}
		else
		{
			lock.unlock();
		}


	}
}

void Myserver::handle_msg(ConnectionMsg & msg)
{
	std::cout << "handle_msg conncetion=" << msg.wsi.lock().get() << " msg=" << msg.msg << std::endl;
	if (msg.msg == "login")
	{
		lock_guard<mutex> guard(m_connection_lock);
		m_connections.insert(msg.wsi);
		m_con_map.insert(std::pair<WSHandle, int>(msg.wsi, m_con_id++));
		std::string str("login sucess");
		m_pServer->SendText(msg.wsi, str.c_str(), str.size());
	}
	else if(msg.msg == "logout")
	{
		lock_guard<mutex> guard(m_connection_lock);
		m_connections.erase(msg.wsi);
		m_con_map.erase(msg.wsi);
		std::string str("logout sucess");
		m_pServer->SendText(msg.wsi, str.c_str(), str.size());
		m_pServer->CloseConnection(msg.wsi);
	}
	else if(msg.msg == "echo")
	{
		std::string str("test echo rsp");
		m_pServer->SendText(msg.wsi, str.c_str(), str.size());
	}
	else if (msg.msg == "stop") 
	{
		{
			lock_guard<mutex> guard(m_connection_lock);
			for (auto& item : m_connections)
			{
				m_pServer->CloseConnection(item);
			}
			m_connections.clear();
			m_con_map.clear();
		}
		//???
		m_pServer->Stop();
	}
	else
	{
		m_pServer->SendText(msg.wsi, msg.msg.c_str(), msg.msg.size());
	}
}
