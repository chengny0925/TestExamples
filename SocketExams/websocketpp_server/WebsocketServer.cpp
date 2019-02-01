#include "WebsocketServer.h"



WebsocketServer::WebsocketServer() : m_pCallBack(nullptr)
{
}


WebsocketServer::~WebsocketServer()
{
	Stop();
}

int WebsocketServer::Init(int port)
{
	m_port = port;
	m_server.set_access_channels(websocketpp::log::alevel::all);
	m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
	//m_server.clear_access_channels(websocketpp::log::alevel::devel);
	//m_server.clear_access_channels(websocketpp::log::alevel::message_payload);
	m_server.clear_access_channels(websocketpp::log::alevel::frame_header);
	m_server.set_error_channels(websocketpp::log::elevel::all);
	//m_server.get_alog().set_ostream();  //重定向log输出

	//todo... error
	m_server.init_asio();

	m_server.set_open_handler(bind(&WebsocketServer::on_open, this, ::_1));
	m_server.set_close_handler(bind(&WebsocketServer::on_close, this, ::_1));
	m_server.set_message_handler(bind(&WebsocketServer::on_message, this, ::_1, ::_2));
	//m_server.set_fail_handler();
	//m_server.set_socket_init_handler();
	m_server.set_ping_handler(bind(&WebsocketServer::on_ping, this, ::_1, ::_2));
	return 0;
}

void WebsocketServer::SetCallBack(IWebsocketServerCallBack * pCallBack)
{
	m_pCallBack = pCallBack;
}

int WebsocketServer::Start()
{
	int ret = 0;
	websocketpp::lib::error_code  ec;
	m_server.listen(m_port, ec);
	if (ec)
	{
		ret = ec.value();
		std::cout << "error_code=" << ec.value() << " error_msg=" << ec.message() << std::endl;
		std::cout << "fail to listen on port " << m_port << std::endl;
	}
	else
	{
		std::cout << "listening on port " << m_port << std::endl;

		try
		{
			// Start the server accept loop
			m_server.start_accept();

			// Start the ASIO io_service run loop
			m_server.run();
		}
		catch (websocketpp::exception const & e) 
		{
			ret = e.code().value();
			std::cout << e.what() << std::endl;
		}
		catch (...) 
		{
			ret = -1;
			std::cout << "other exception" << std::endl;
		}

	}

	return ret;
}

void WebsocketServer::Stop()
{
	m_server.stop();
}

void WebsocketServer::CloseConnection(WSHandle wsi)
{
	websocketpp::lib::error_code ec;
	std::string reson;
	m_server.close(wsi, websocketpp::close::status::policy_violation, reson, ec);
	//todo...
	if (ec)
	{
		m_server.get_alog().write(websocketpp::log::elevel::warn, "close connection error");
	}
}

int WebsocketServer::SendText(WSHandle wsi, const char * message, int len)
{
	websocketpp::lib::error_code ec;
	m_server.send(wsi, message, len, websocketpp::frame::opcode::value::TEXT, ec);
	return ec.value();
}

int WebsocketServer::SendBinary(WSHandle wsi, const char * message, int len)
{
	websocketpp::lib::error_code ec;
	m_server.send(wsi, message, len, websocketpp::frame::opcode::value::BINARY, ec);
	return ec.value();
}

void WebsocketServer::on_open(connection_hdl hdl)
{
	std::cout << "on_open: " << hdl.lock().get() << std::endl;
	auto p = m_server.get_con_from_hdl(hdl);
	auto remote = p->get_remote_endpoint();
	std::cout << "remote endpoint: " << remote << std::endl;
	if (m_pCallBack != nullptr)
	{
		m_pCallBack->NewConnection(hdl);
	}
}

void WebsocketServer::on_close(connection_hdl hdl)
{
	std::cout << "on_close: " << hdl.lock().get() << std::endl;
	if (m_pCallBack != nullptr)
	{
		m_pCallBack->Disconnection(hdl); 
	}
}

void WebsocketServer::on_message(connection_hdl hdl, server::message_ptr msg)
{
	if (m_pCallBack != nullptr)
	{
		if (msg->get_opcode() == websocketpp::frame::opcode::value::TEXT)
		{
			m_pCallBack->TextReceived(hdl, msg->get_payload().c_str(), msg->get_payload().size());
		}
		else if (msg->get_opcode() == websocketpp::frame::opcode::value::BINARY)
		{
			m_pCallBack->BinaryReceived(hdl, msg->get_payload().c_str(), msg->get_payload().size());
		}
		
	}
}

bool WebsocketServer::on_ping(connection_hdl hdl, std::string payload)
{
	return true;
}
