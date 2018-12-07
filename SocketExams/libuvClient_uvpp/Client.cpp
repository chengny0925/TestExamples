#include "Client.h"
#include "uvpp/make_unique_define.h"
#include <iostream>
using namespace std;

Client::Client() : m_port(0), m_is_connected(false), m_loop(), m_tcp_conn(m_loop), m_on_error(), m_on_connect(), m_async(m_loop, bind(&Client::on_async, this))
{
}


Client::~Client()
{
	m_is_connected = false;

	m_send_lock.lock();
	while (!m_sendBuf.empty())
	{
		m_sendBuf.pop_front();
	}
	m_send_lock.unlock();

	Close();

	if (m_p_thread)
	{
		m_p_thread->join();
		delete m_p_thread;
	}
	//if (m_loop.get())
	//	m_loop.stop();
	//(cout << "release success!" << std::endl);
}

int Client::Connect(const char * ip, int port)
{
	m_tcp_conn.connect(ip, port,std::bind(&Client::on_tcp_connect, this, placeholders::_1));
	m_p_thread = new std::thread(&Client::RunThread, this);
	return 0;
}

void Client::Send(const char * msg, int size)
{
	if (!m_is_connected)
	{
		return;
	}

	std::lock_guard<std::mutex> l(m_send_lock);
	m_sendBuf.push_back(msg);
	m_async.send();
}

void Client::Close()
{
	if (m_is_closing)
		return;
	m_is_closing = true;
	m_is_connected = false;
	m_async.send();
}

void Client::RunThread()
{
	m_loop.run();
}

void Client::OnMsg(TcpConnection * client, const char * msg, int size)
{
	cout << "OnMsg:" << string(msg, size) << endl;
}

void Client::OnClose()
{
	m_is_connected = false;
	m_loop.stop();
}

void Client::OnError()
{
}

void Client::on_tcp_connect(uvpp::error error)
{
	if (bool(error))
	{
		cout << "on_tcp_connect:" << error.str() << " " << bool(error) << endl;
		OnError();
	}
	else
	{
		m_is_connected = true;
	}

	m_tcpConnectionPtr = make_unique<TcpConnection>(m_loop);
	TcpConnection &tcp_conn = *m_tcpConnectionPtr;

	string peer_ip;
	int port;
	bool ip4;
	const bool getpeername_ok = m_tcp_conn.getpeername(ip4, peer_ip, port);
	assert(getpeername_ok);
	(void)getpeername_ok;
	ostringstream os;
	os << "tcp://" << peer_ip << ":" << port;
	string peer_ = os.str();
	cout << "connect:" << peer_ << endl;
	const string &peer = peer_;

	auto close_cb = [this, &peer]() {
		//if (m_on_close)
		//	m_on_close(peer);
		cout << "on_tcp_close:" << peer << endl;
		OnClose();
	};

	// write finished callback
	auto write_cb = [this, &tcp_conn, close_cb](uvpp::error error) {
		if (!error)
		{
			tcp_conn.on_write_finished();
		}
		else
		{
			cout << "write_cb error:" << error.str() << endl;
			m_tcp_conn.close(close_cb);
		}
	};

	// function to be called when the the protocol needs to write
	auto do_write = [this, write_cb](const char *buff, size_t sz) {
		bool ok = m_tcp_conn.write(buff, sz, write_cb);
		if (!ok)
		{
			cout << "uv_write error:"/* << m_tcp_conn.get()->*/ << endl;
			OnError();
		}
	};

	auto read_cb = [this, &tcp_conn, &peer, close_cb](const char *buff, ssize_t len) {
		if (len < 0)
		{
			cerr << "TCP client read error: " << peer << " error:" << uvpp::error(len).str() << endl;
			m_tcp_conn.close(close_cb);
		}
		else
		{
			tcp_conn.input(buff, static_cast<size_t>(len));
		}
	};

	tcp_conn.set_write_fun(do_write);

	tcp_conn.set_msg_cb(std::bind(&Client::OnMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	bool ok = m_tcp_conn.read_start(read_cb);
	if (!ok)
	{
		cout << "uv read_start error:"/* << m_tcp_conn.get()->*/ << endl;
		OnError();
	}
}

void Client::on_async()
{
	if (m_is_closing)
	{
		auto close_cb = std::bind(&Client::OnClose, this);
		m_tcp_conn.close(close_cb);
		//uv_walk(m_loop, WalkCallBack, this);
		return;
	}

	std::lock_guard<std::mutex> l(m_send_lock);
	while (!m_sendBuf.empty())
	{
		const std::string &buf = m_sendBuf.front();

		m_tcpConnectionPtr->send_msg(move(buf));
		m_sendBuf.pop_front();
	}
}
