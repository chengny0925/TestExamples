#include "RJTcpServer.h"
#include <sstream>
#include <algorithm>

using namespace std;

RJTcpServer::RJTcpServer()
{
	m_callback = nullptr;
	m_pLoop = nullptr;
	m_p_thread = nullptr;
	m_is_closing = false;
}


RJTcpServer::~RJTcpServer()
{
	m_send_buf_lock.lock();
	while (!m_send_buf.empty())
	{
		delete m_send_buf.front().buf.base;
		m_send_buf.pop();
	}
	m_send_buf_lock.unlock();

	Close();

	if (m_p_thread)
	{
		m_p_thread->join();
		delete m_p_thread;
	}

	if (m_pLoop)
		uv_loop_close(m_pLoop);

	for (auto i = m_clients.begin(); i != m_clients.end(); i++)
	{
		delete *i;
	}

	STDOUT(cout << "server release success!" << endl;)
}

//msg指向的内存直接拷贝到new出的内存中，msg由发送者自己维护
int RJTcpServer::Send(uv_tcp_t* client, const char* msg, int size)
{
	if (m_is_closing || client == nullptr || msg == nullptr || size <= 0)return -1;

	char* buf = new char[size + PACKAGE_HEAD_SIZE];
	Msg2Package(msg, size, buf);

	uv_tcp_send_buf uv_buf;
	uv_buf.buf.base = buf;
	uv_buf.buf.len = size + PACKAGE_HEAD_SIZE;
	uv_buf.type = uv_send_type::Single;
	uv_buf.client = client;

	m_send_buf_lock.lock();
	m_send_buf.push(uv_buf);
	m_send_buf_lock.unlock();
	return uv_async_send(&m_async_handle);
}

//void RJTcpServer::Broadcast(const char * msg, int size)
//{
//	if (m_is_closing || msg == nullptr || size <= 0)
//		return;
//
//	char* buf = new char[size + PACKAGE_HEAD_SIZE];
//	Msg2Package(msg, size, buf);
//
//	uv_tcp_send_buf uv_buf;
//	uv_buf.buf.base = buf;
//	uv_buf.buf.len = size + PACKAGE_HEAD_SIZE;
//	uv_buf.type = uv_send_type::Broadcast;
//	uv_buf.client = nullptr;
//
//	m_send_buf_lock.lock();
//	m_send_buf.push(uv_buf);
//	m_send_buf_lock.unlock();
//	uv_async_send(&m_async_handle);
//}

void RJTcpServer::OnMsg(uv_tcp_t* client, const char* msg, int size)
{
	//STDOUT(cout << "Client-" << client << ':' << msg << endl;)
	//STDOUT(cout << msg + 16 << endl;)

	if (msg == nullptr || size < 0)
	{
		auto result = RemoveClient(client);
		if (result == 0)
		{
			SendLog("Unpackage error:", "remove this connection!");
			uv_close((uv_handle_t*)client, ClientClose);
		}
		return;
	}


	if (m_callback)
	{
		for (int i = 0; i < m_clients.size(); i++)
		{
			if (&m_clients[i]->client == client)
			{
				m_callback->OnMsg(m_clients[i], msg, size);
				break;
			}
		}
	}

}

void RJTcpServer::OnNewConnection(uv_tcp_client* client)
{
	stringstream ss;
	ss << "OnNewConnection:";
	ss << client;
	SendLog(ss.str().c_str(), "");
	if (m_callback)
		m_callback->OnNewConnection(client);
}

void RJTcpServer::OnDisconnection(uv_tcp_client* client)
{
	stringstream ss;
	ss << "OnDisconnection:";
	ss << client;
	SendLog(ss.str().c_str(), "");

	if (m_callback)
		m_callback->OnDisconnection(client);
}


int RJTcpServer::RemoveClient(uv_tcp_t* client)
{
	int result = -1;
	for (auto i = m_clients.begin(); i != m_clients.end(); i++)
	{
		if (&(*i)->client == client)
		{
			OnDisconnection(*i);
			m_clients.erase(i);
			result = 0;
			break;
		}
	}

	return result;
}

bool RJTcpServer::CheckClient(uv_tcp_t * client)
{
	for (auto i = m_clients.begin(); i != m_clients.end(); i++)
	{
		if (&(*i)->client == client)
		{
			return true;
		}
	}
	return false;
}

void RJTcpServer::DeleteWriteReq(uv_write_t * write)
{
	write_req_t* req = (write_req_t*)write;
	delete[] req->buf.base;
	delete req;
}

void RJTcpServer::AfterSend(uv_write_t* req, int status)
{
	if (status)
	{
		uv_tcp_t* client = (uv_tcp_t*)req->data;
		auto server = (RJTcpServer*)client->data;
		auto result = server->RemoveClient(client);
		if (result == 0)
		{
			auto error = GetUVError(status);
			server->SendLog("AfrerSend:", error.c_str());
			uv_close((uv_handle_t*)client, ClientClose);
		}
	}
	DeleteWriteReq(req);
}

//void RJTcpServer::AfterBroadcast(uv_write_t * req, int status)
//{
//	if (status)
//	{
//		uv_tcp_client* client = (uv_tcp_client*)req->data;
//		auto server = (RJTcpServer*)client->client.data;
//		
//		//把无效的client放入带删除列表
//		auto iter = std::find(server->m_clients4delete.begin(), server->m_clients4delete.end(), client);
//		if (iter == server->m_clients4delete.end())
//		{
//			auto error = GetUVError(status);
//			server->SendLog("AfterBroadcast:", error.c_str());
//			server->m_clients4delete.push_back(client);
//		}
//	}
//	//只释放write_req_t结构体，里面的buffer内容在广播遍历结束后释放
//	delete (write_req_t*)req;
//}

void RJTcpServer::AllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

void RJTcpServer::HandleMsg(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	if (nread > 0)
	{
		((uv_tcp_client*)client)->reader.SetBuffer(buf->base, nread);
	}
	else
	{
		auto server = ((RJTcpServer*)client->data);
		auto error = GetUVError(nread);
		server->SendLog("HandleMsg:", error.c_str());
		auto result = server->RemoveClient((uv_tcp_t*)client);
		if (result == 0)
			uv_close((uv_handle_t*)client, ClientClose);
	}

	free(buf->base);
}

void RJTcpServer::ClientClose(uv_handle_t* handle)
{
	uv_tcp_client* pClient = (uv_tcp_client*)handle;
	delete pClient;
}

void RJTcpServer::AcceptConnection(uv_stream_t* server, int status)
{
	RJTcpServer* rj_server = (RJTcpServer*)server->data;

	if (status)
	{
		auto error = GetUVError(status);
		rj_server->SendLog("AcceptConnection:", error.c_str());
		return;
	}

	if (rj_server->m_is_closing)
		return;

	int iret;
	uv_tcp_client* client = new uv_tcp_client();
	client->reader.SetCallBack(std::bind(&RJTcpServer::OnMsg, rj_server, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		, &client->client);
	client->client.data = rj_server;

	iret = uv_tcp_init(rj_server->m_pLoop, &client->client);
	if (iret)
	{
		auto error = GetUVError(iret);
		rj_server->SendLog("AcceptConnection->uv_tcp_init:", error.c_str());
		delete client;
		return;
	}

	iret = uv_accept(server, (uv_stream_t*)client);
	if (iret)
	{
		auto error = GetUVError(iret);
		rj_server->SendLog("AcceptConnection->uv_accept:", error.c_str());
		uv_close((uv_handle_t*)client, ClientClose);
		return;
	}

	iret = uv_read_start((uv_stream_t*)client, AllocBuffer, HandleMsg);
	if (iret)
	{
		auto error = GetUVError(iret);
		rj_server->SendLog("AcceptConnection->uv_read_start:", error.c_str());
		uv_close((uv_handle_t*)client, ClientClose);
		return;
	}
	rj_server->m_clients.push_back(client);
	rj_server->OnNewConnection(client);
}

void RJTcpServer::AsyncCallBack(uv_async_t * handle)
{
	RJTcpServer* server = (RJTcpServer*)handle->data;

	//遍历当前的连接并关闭
	if (server->m_is_closing)
	{
		uv_walk(server->m_pLoop, WalkCallBack, server);
		return;
	}

	//移除上次广播中无效的客户端
	//auto iter = server->m_clients4delete.begin();
	//if (iter != server->m_clients4delete.end())
	//{
	//	auto result = server->RemoveClient(&(*iter)->client);
	//	if (result == 0)
	//		uv_close((uv_handle_t*)&(*iter)->client, ClientClose);
	//	iter++;
	//}
	//server->m_clients4delete.clear();

	static std::queue<uv_tcp_send_buf> sendTemp;

	server->m_send_buf_lock.lock();
	sendTemp.swap(server->m_send_buf);
	server->m_send_buf_lock.unlock();

	while (!sendTemp.empty())
	{
		uv_tcp_send_buf& buf = sendTemp.front();

		if (buf.type == uv_send_type::Single)
		{
			if (!server->CheckClient(buf.client))
			{
				delete[] buf.buf.base;
			}
			else
			{
				write_req_t *req = new write_req_t();
				req->req.data = buf.client;
				req->buf = uv_buf_init(buf.buf.base, buf.buf.len);
				int iret;
				iret = uv_write(&req->req, (uv_stream_t*)buf.client, &req->buf, 1, AfterSend);
				if (iret)
				{
					auto error = GetUVError(iret);
					server->SendLog("AsyncCallBack->Single->uv_write:", error.c_str());
					int result = server->RemoveClient(buf.client);
					if (result == 0)
						uv_close((uv_handle_t*)buf.client, ClientClose);
					DeleteWriteReq((uv_write_t*)req);
				}
			}
		}
		//else
		//{
		//	auto iter = server->m_clients.begin();

		//	while (iter != server->m_clients.end())
		//	{
		//		write_req_t *req = new write_req_t();
		//		req->req.data = &(*iter)->client;
		//		req->buf = uv_buf_init(buf.buf.base, buf.buf.len);
		//		int iret;
		//		iret = uv_write(&req->req, (uv_stream_t*)&(*iter)->client, &req->buf, 1, AfterBroadcast);
		//		if (iret)
		//		{
		//			auto error = GetUVError(iret);
		//			server->SendLog("AsyncCallBack->Broadcast->uv_write:", error.c_str());
		//			server->OnDisconnection(&(*iter)->client);
		//			server->m_clients.erase(iter++);
		//			uv_close((uv_handle_t*)buf.client, ClientClose);
		//			delete req;
		//		}
		//		else
		//		{
		//			iter++;
		//		}
		//	}
		//	//遍历结束，释放buf
		//	delete[] buf.buf.base;
		//}
		sendTemp.pop();
	}
}

void RJTcpServer::WalkCallBack(uv_handle_t* handle, void* arg)
{
	if (!uv_is_closing(handle)) {
		uv_close(handle, nullptr);
	}
}

///关闭所有监听器，包括监听的所有TcpClient
void RJTcpServer::Close()
{
	if (m_is_closing)
		return;

	m_is_closing = true;

	if (m_pLoop)
	{
		uv_async_send(&m_async_handle);
	}
}

int RJTcpServer::Init(int port)
{
	int iret;
	sockaddr_in addr;
	m_pLoop = uv_default_loop();

	m_async_handle.data = this;
	iret = uv_async_init(m_pLoop, &m_async_handle, AsyncCallBack);
	if (iret)
	{
		auto error = GetUVError(iret);
		SendLog("Init -> uv_async_init:", error.c_str());
		return -1;
	}
	m_server.data = this;
	iret = uv_tcp_init(m_pLoop, &m_server);
	if (iret)
	{
		auto error = GetUVError(iret);
		SendLog("Init->uv_tcp_init:", error.c_str());
		return -2;
	}
	iret = uv_ip4_addr(DEFAULT_IP, port, &addr);
	if (iret)
	{
		auto error = GetUVError(iret);
		SendLog("Init->uv_ip4_addr:", error.c_str());
		return -3;
	}
	iret = uv_tcp_bind(&m_server, (const sockaddr*)&addr, 0);
	if (iret)
	{
		auto error = GetUVError(iret);
		SendLog("Init->uv_tcp_bind:", error.c_str());
		return -4;
	}
	iret = uv_listen((uv_stream_t*)&m_server, BACK_LOG, AcceptConnection);
	if (iret)
	{
		auto error = GetUVError(iret);
		SendLog("Init->uv_listen:", error.c_str());
		return -5;
	}

	m_p_thread = new thread(&RJTcpServer::RunThread, this);
	return 0;
}

void RJTcpServer::SendLog(const char * source, const char * errorMsg)
{
	if (m_callback != nullptr)
	{
		std::string msg;
		msg.append(source);
		msg.append(errorMsg);
		m_callback->OnLog(msg.c_str());
	}
}

void RJTcpServer::RunThread()
{
	uv_run(m_pLoop, UV_RUN_DEFAULT);
}

void IRJTcpServerCallBack::OnLog(const char * log)
{
	STDOUT(cout << log << endl);
}
