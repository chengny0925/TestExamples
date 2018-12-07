#include "Server.h"
#include "uvpp/make_unique_define.h"
#include <iostream>

using namespace std;

Server::Server() :  m_port(0), m_running(false), m_loop(), m_tcp_listen_conn(m_loop), m_on_close(), m_on_connect(), m_async(m_loop, bind(&Server::on_async, this))
{

}

Server::~Server()
{
	if (m_p_thread)
	{
		m_p_thread->join();
		delete m_p_thread;
	}
}

void Server::set_port(int port)
{
	if (m_running)
		throw std::runtime_error("Daemon::set_port can't change while running");
	m_port = port;
}

void Server::start()
{
	std::cout << "server start listening :12345" << std::endl;
	m_tcp_listen_conn.bind("0.0.0.0", 12345);
	m_tcp_listen_conn.listen(std::bind(&Server::on_tcp_connect, this, placeholders::_1));
	m_running = true;
	m_p_thread = new std::thread(&Server::RunThread, this);
}

void Server::stop()
{
	m_running = false;
}

void Server::send(const string &peer, const string &msg)
{
	std::lock_guard<std::mutex> l(m_send_lock);
	m_sendBuf.push_back(msg_buf(peer, msg));
	m_async.send();
}

void Server::Broadcast(const std::string &msg)
{
	std::lock_guard<std::mutex> l(m_send_lock);
	auto iter = m_connections.begin();
	while (iter != m_connections.end())
	{
		m_sendBuf.push_back(msg_buf(iter->first, msg));
		iter++;
	}
	m_async.send();
}

void Server::on_async()
{
	std::lock_guard<std::mutex> l(m_send_lock);
	while (!m_sendBuf.empty())
	{
		const msg_buf &buf = m_sendBuf.front();
		auto iter = m_connections.find(buf.peer);
		if (iter != m_connections.end())
		{
			iter->second->send_msg(move(buf.msg));
		}
		m_sendBuf.pop_front();
	}
}

void Server::RunThread()
{
	m_loop.run();
}

void Server::OnMsg(TcpConnection * client, const char * msg, int size)
{
	cout << "OnMsg:" << client << ", "  << string(msg, size) << endl;
}

void Server::OnClose(const std::string & peer)
{
	m_connections.erase(peer);
	cout << "on_tcp_close: " << peer /*<< error.str() << " " << bool(error)*/ << endl;
}

void Server::on_tcp_connect(uvpp::error error)
{
	if (bool(error))
	{
		cout << "on_tcp_connect:" << error.str() << endl;
	}

	auto tcp_conn_ptr = make_unique<TcpConnection>(m_loop);
	TcpConnection &tcp_conn = *tcp_conn_ptr;
	m_tcp_listen_conn.accept(tcp_conn.m_tcp);

	string peer_ip;
	int port;
	bool ip4;
	const bool getpeername_ok = tcp_conn.m_tcp.getpeername(ip4, peer_ip, port);
	assert(getpeername_ok);
	(void)getpeername_ok;
	ostringstream os;
	os << "tcp://" << peer_ip << ":" << port;
	string peer_ = os.str();

	tcp_conn.set_peer_name(peer_);

	auto res = m_connections.emplace(piecewise_construct,
																	 forward_as_tuple(move(peer_)),
																	 forward_as_tuple(move(tcp_conn_ptr)));

	assert(res.second);
	const string &peer = res.first->first; // reference that will stay valid

	//if (m_on_connect)
	//{
	//	m_on_connect(peer);
	//}

	cout << "receive new connect:" << peer << " size = " << m_connections.size() << endl;

	auto close_cb = [this, peer]() {
		//if (m_on_close)
		//	m_on_close(peer);
		//cout << "erase peer " << peer << "  connect: size = " << m_connections.size() << endl;
		m_connections.erase(peer);
		cout << "on_tcp_close:" << "  connect: size = " << m_connections.size() << endl;

	};

	//auto close_cb = std::bind(&Server::OnClose, this, placeholders::_1);

	// write finished callback
	auto write_cb = [&tcp_conn, close_cb](uvpp::error error) {
		if (!error)
		{
			tcp_conn.on_write_finished();
		}
		else
		{
			cout << "write_cb error:" << error.str() << endl;
			tcp_conn.m_tcp.close(close_cb);
		}
	};

	// function to be called when the the protocol needs to write
	auto do_write = [&tcp_conn, write_cb, close_cb](const char *buff, size_t sz) {
		bool ok = tcp_conn.m_tcp.write(buff, sz, write_cb);
		if (!ok)
		{
			cout << "uv_write error:"/* << m_tcp_conn.get()->*/ << endl;
			tcp_conn.m_tcp.close(close_cb);
		}
	};

	auto read_cb = [&tcp_conn, &peer, close_cb](const char *buff, ssize_t len) {
		if (len < 0)
		{
			cerr << "TCP client read error: " << peer << " error:" << uvpp::error(len).str() << endl;
			tcp_conn.m_tcp.close(close_cb);
		}
		else
		{
			tcp_conn.input(buff, static_cast<size_t>(len));
		}
	};

	tcp_conn.set_write_fun(do_write);

	tcp_conn.set_msg_cb(std::bind(&Server::OnMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	tcp_conn.m_tcp.read_start(read_cb);
}
