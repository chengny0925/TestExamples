#include "stdafx.h"
#include "WebsocketServer.h"
#include "WebsockThread.h"
#include <libwebsockets.h>
#include <mutex>
#include <stdio.h>
#include <assert.h>
#include  <log4cxx/log4cxx.h>
#include <log4cxx/logger.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVRE_CACHE_MAX   16*1024*1024

LOGGER(logAppender, "WebsocketServer");

__declspec(thread) static WebsocketServer *this_web_server_;
__declspec(thread) static bool bBusy_;

struct MsgHead 
{
	enum Type {Text, Binary, Ping, Close};
	Type type_;
	int len_;
};

struct CacheItem
{
	char peerAddr_[64];
	int port_;
	std::string sendBuf_;
	bool updated_;
	bool sending_;
	CacheItem() : port_(0), updated_(false), sending_(false)
	{
		peerAddr_[sizeof(peerAddr_) - 1] = 0;
		sendBuf_.reserve(64 * 1024);
	}
};

struct ServerInOut
{
	std::string receive_;
	std::string send_;
	int curSendPos_;
	bool bframe_;
	bool isBinary_;
	
	ServerInOut() : curSendPos_(0), bframe_(false), isBinary_(false)
	{
		send_.reserve(64 * 1024);
		receive_.reserve(64 * 1024);
	}
};

std::map<void *, CacheItem> send_cache_;
std::recursive_mutex send_mutex_;
static bool server_updated_ = false;

//__declspec(thread) std::map<void*, ServerInOut> server_inout_;

struct ServerMetaData
{
	std::vector<WebsockThread*> threads_;
	lws_context *server_context_;
	bool         stopped_;

	ServerMetaData() : server_context_(0), stopped_(false) {}
};


int strPeerName(SOCKET s, char *ip, int ipLen, int &port)
{
	struct sockaddr_in6 perrAddr;
	socklen_t len = sizeof perrAddr;
	int err = ::getpeername((SOCKET)s, (sockaddr*)&perrAddr, &len);
	if (err)
	{
		LOG4CXX_ERROR(logAppender, "getpeername() " LOG_VAR(__LINE__) LOG_VAR(err));
		return 1;
	}

	switch (perrAddr.sin6_family)
	{
	case AF_INET:
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&perrAddr)->sin_addr),
			ip, ipLen);
		port = ((struct sockaddr_in *)&perrAddr)->sin_port;
		break;
	case AF_INET6:
		inet_ntop(AF_INET6, &perrAddr.sin6_addr,
			ip, ipLen);
		port = perrAddr.sin6_port;
		break;
	default:
		return 1;
	}

	ip[ipLen - 1] = 0;

	return 0;
}

static int server_callback(struct lws *wsi, enum lws_callback_reasons reason,
	void *user, void *in, size_t len)
{
	
	switch (reason)
	{
	case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
	{
		bBusy_ = true;
		char peerName[128];
		int port = 0;

		if (strPeerName((SOCKET)in, peerName, sizeof peerName, port))
			return 1;

		if (this_web_server_)
			return this_web_server_->BeforeAccept(peerName);
		break;
	}	
	case LWS_CALLBACK_ESTABLISHED:
	{
		bBusy_ = true;
		assert(user);
		ServerInOut * &pinout = *(ServerInOut **)user;
		pinout = new (std::nothrow) ServerInOut;
		if (pinout)
		{
			SOCKET sockfd = lws_get_socket_fd(wsi);
			char peerName[128];
			int port = 0;
			strPeerName((SOCKET)sockfd, peerName, sizeof peerName, port);
			peerName[sizeof(peerName) - 1] = 0;

			send_mutex_.lock();

			CacheItem &item = send_cache_[wsi];
			strncpy_s(item.peerAddr_, peerName, sizeof(item.peerAddr_) - 1);
			item.port_ = port;

			send_mutex_.unlock();

			LOG4CXX_INFO(logAppender, "LWS_CALLBACK_ESTABLISHED connect" LOG_VAR(__LINE__) LOG_VAR(peerName) LOG_VAR(port));

			this_web_server_->NewConnection(wsi, peerName, port);
		}
		
		break;
	}
	case LWS_CALLBACK_RECEIVE:
	{
		bBusy_ = true;
		ServerInOut * &pinout = *(ServerInOut **)user;
		if (pinout)
		{
			int isFirst = lws_is_first_fragment(wsi);
			int isFinal = lws_is_final_fragment(wsi);
			int isBinary = lws_frame_is_binary(wsi);

			const size_t remaining = lws_remaining_packet_payload(wsi);

			if (remaining > SERVRE_CACHE_MAX)
			{
				LOG4CXX_WARN(logAppender, "if (remaining > SERVRE_CACHE_MAX)" LOG_VAR(__LINE__) LOG_VAR(wsi) LOG_VAR(remaining));
				return -1;
			}

			auto &inout = *pinout;

			auto old = inout.receive_.size();
			inout.receive_.resize(old + len);
			memcpy(&inout.receive_[old], in, len);

			if (isFirst)
			{
				inout.bframe_ = true;
				inout.isBinary_ = isBinary != 0;
				break;
			}
			if (!isFinal)
			{
				break;
			}

			assert(this_web_server_);
			if ( 0 == remaining && this_web_server_ )
			{
				if (!inout.bframe_)
				{
					if (isBinary)
						this_web_server_->BinaryReceived(wsi, inout.receive_.data(), inout.receive_.size());
					else
						this_web_server_->TextReceived(wsi, inout.receive_.data(), inout.receive_.size());
				}
				else
				{
					if (inout.isBinary_)
						this_web_server_->BinaryFrameReceived(wsi, inout.receive_.data(), inout.receive_.size(), 0 != isFinal);
					else
						this_web_server_->TextFrameReceived(wsi, inout.receive_.data(), inout.receive_.size(), 0 != isFinal);
				}

				inout.receive_.resize(0);
				inout.bframe_ = false;
			}
		}
		
		break;
	}
	case LWS_CALLBACK_RECEIVE_PONG:
	{
		bBusy_ = true;
		assert(this_web_server_);
		uint64_t timeElapsed = 0;
		if (this_web_server_)
			this_web_server_->Pong(wsi, timeElapsed, (char*)in, len);

		break;
	}
	case LWS_CALLBACK_SERVER_WRITEABLE:
	{
		bBusy_ = true;
		ServerInOut * &pinout = *(ServerInOut **)user;
		if (pinout)
		{
			auto &inout = *pinout;
			bool exist = true;
			if (inout.send_.size() < inout.curSendPos_ + sizeof(MsgHead))
			{
				inout.curSendPos_ = 0;
				inout.send_.resize(0);
				send_mutex_.lock();

				auto it = send_cache_.find(wsi);
				if (it != send_cache_.end())
				{
					it->second.sendBuf_.swap(inout.send_);
					if (inout.send_.size() <= 0)
					{
						it->second.sending_ = false;
						exist = false;
					}
					else
					{
						it->second.sending_ = true;
					}
				}
				else
					exist = false;

				send_mutex_.unlock();
			}

			if (!exist)
				return 0;

			if (inout.send_.size() - inout.curSendPos_ > SERVRE_CACHE_MAX)
			{
				int BytesCached = inout.send_.size() - inout.curSendPos_;
				LOG4CXX_INFO(logAppender, "Connection cache full" LOG_VAR(__LINE__)  LOG_VAR(BytesCached) );
				
				return -1;
			}

			while (inout.send_.size() >= sizeof(MsgHead) + inout.curSendPos_)
			{
				MsgHead *pHead = (MsgHead*)&inout.send_[inout.curSendPos_];
				bool closed = false;
				switch (pHead->type_)
				{
				case MsgHead::Close:
				{
					return -1;
				}
				case MsgHead::Text:
				{
					assert(pHead->len_ >= LWS_PRE);
					lws_write(wsi, (unsigned char*)&inout.send_[inout.curSendPos_ + sizeof(MsgHead) + LWS_PRE], pHead->len_ - LWS_PRE, LWS_WRITE_TEXT);
					//lws_callback_on_writable(wsi);
					break;
				}
				case MsgHead::Binary:
				{
					assert(pHead->len_ >= LWS_PRE);
					lws_write(wsi, (unsigned char*)&inout.send_[inout.curSendPos_ + sizeof(MsgHead) + LWS_PRE], pHead->len_ - LWS_PRE, LWS_WRITE_BINARY);
					
					break;
				}
				case MsgHead::Ping:
				{
					assert(pHead->len_ >= LWS_PRE);
					lws_write(wsi, (unsigned char*)&inout.send_[inout.curSendPos_ + sizeof(MsgHead) + LWS_PRE], pHead->len_ - LWS_PRE, LWS_WRITE_PING);
					//lws_callback_on_writable(wsi);
					break;
				}

				default:
					assert(0);
					break;
				}

				inout.curSendPos_ += sizeof(MsgHead) + pHead->len_;
				if (inout.curSendPos_ >= inout.send_.size())
				{
					inout.curSendPos_ = 0;
					inout.send_.resize(0);
					if(lws_partial_buffered(wsi) != 1)
						lws_callback_on_writable(wsi);
					break;
				}

				if(closed || lws_partial_buffered(wsi) == 1)
					break;
			}
		}
		
		break;
	}
	case LWS_CALLBACK_CLOSED:
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
	{
		bBusy_ = true;
		SOCKET sockfd = lws_get_socket_fd(wsi);
		char peerName[128];
		int port = 0;
		strPeerName((SOCKET)sockfd, peerName, sizeof peerName, port);
		peerName[sizeof(peerName) - 1] = 0;

		LOG4CXX_INFO(logAppender,  LOG_VAR(reason) LOG_VAR(wsi) LOG_VAR(sockfd) LOG_VAR(peerName) LOG_VAR(port)  );

		break;
	}
	case LWS_CALLBACK_WSI_DESTROY:
	{
		bBusy_ = true;
		ServerInOut * &pinout = *(ServerInOut **)user;
		
		bool exist = false;
		char peerName[128];
		int port = 0;

		send_mutex_.lock();

		auto it = send_cache_.find(wsi);
		if (it != send_cache_.end())
		{
			exist = true;
			strncpy_s(peerName, it->second.peerAddr_, sizeof(peerName) - 1);
			port = it->second.port_;
			send_cache_.erase(wsi);
		}
		send_mutex_.unlock();

		if (user && pinout)
			delete pinout;

		if (exist)
		{
			LOG4CXX_INFO(logAppender, "LWS_CALLBACK_WSI_DESTROY disconnect " LOG_VAR(__LINE__) LOG_VAR(peerName) LOG_VAR(port));
			this_web_server_->Disconnection(wsi, peerName, port);
		}
		break;
	}
		
	}

	return 0;
}

static struct lws_protocols server_protocols[] = 
{
	{
		"server-protocol", // protocol name - very important!
		server_callback,   // callback
		sizeof(void*)
	},
	{
		NULL, NULL, 0   /* End of list */
	}
};


WebsocketServer::WebsocketServer()
	: port_(0), thread_num_(0), thread_priority_(Idle), meta_data_(nullptr), m_callBack(nullptr)
{

}

WebsocketServer::~WebsocketServer()
{
	if (meta_data_)
		delete (ServerMetaData*)meta_data_;
}

class ServerThread : public WebsockThread
{
public:
	ServerThread(WebsocketServer *server, ThreadPriority prior = Normal) : WebsockThread(prior), server_(server)
	{

	}
	virtual int run() override      //  Ïß³Ìº¯Êý
	{
		this_web_server_ = server_;
		

		ServerMetaData *meta = (ServerMetaData*)server_->MetaData();
		
		std::vector<lws*> updateds;
		updateds.reserve(1024);

		while (!meta->stopped_)
		{
			if (server_updated_)
			{
				updateds.clear();
				send_mutex_.lock();

				for (auto &item : send_cache_)
				{
					if (!item.second.sending_ && item.second.updated_)
					{
						item.second.updated_ = false;
						updateds.push_back((lws*)item.first);
					}
				}
				server_updated_ = false;
				send_mutex_.unlock();

				for (auto &item : updateds)
				{
					lws_callback_on_writable(item);
				}
				//updateds.clear();
			}

			int cnt = 10;

			do 
			{
				bBusy_ = false;
				lws_service(meta->server_context_, 0);
			} while (bBusy_ && cnt--);
			
			if (!server_updated_)
				::Sleep(1);
		}
			
		return 0;
	}

private:
	WebsocketServer *server_;
};

int WebsocketServer::Init(int port, ThreadPriority prior, unsigned threadNum)
{
	port_ = port;
	thread_num_ = threadNum;
	thread_priority_ = prior;
	meta_data_ = new ServerMetaData();
	if (thread_num_ < 1)
		thread_num_ = 1;
	return 0;
}

void WebsocketServer::SetCallBack(IWebsocketServerCallBack * pCallBack)
{
	m_callBack = pCallBack;
}

int WebsocketServer::Start()
{
	lws_context_creation_info info = { 0 };
	
	info.port = port_;
	info.protocols = server_protocols;

	info.gid = -1;
	info.uid = -1;
	info.options = LWS_SERVER_OPTION_VALIDATE_UTF8;
	info.timeout_secs = 30;

	struct lws_context *context = lws_create_context(&info);
	if (0 == context)
	{
		LOG4CXX_ERROR(logAppender, "lws_create_context() failed " LOG_VAR(__LINE__));
		return -1;
	}

	ServerMetaData *pmeta = (ServerMetaData*)meta_data_;
	pmeta->server_context_ = context;

	thread_num_ = 1;
	for (unsigned i = 0; i < thread_num_; ++i)
	{
		WebsockThread *pth = new (std::nothrow) ServerThread(this,(WebsockThread::ThreadPriority)thread_priority_);
		if (pth)
		{
			if (pth->start())
			{
				pmeta->threads_.push_back(pth);
			}
			else
			{
				delete pth;
			}
		}
	}

	LOG4CXX_INFO(logAppender, "WebsocketServer successfully " LOG_VAR(__LINE__) LOG_VAR(this) LOG_VAR(port_));


 	for (unsigned i = 0; i < pmeta->threads_.size(); ++i)
		pmeta->threads_[i]->Join();

	LOG4CXX_INFO(logAppender, "WebsocketServer normally quit " LOG_VAR(__LINE__) LOG_VAR(this) LOG_VAR(port_));

	return 0;
}

void WebsocketServer::Stop()
{
	ServerMetaData *pmeta = (ServerMetaData*)meta_data_;
	pmeta->stopped_ = true;
}



static int MetaSend(WSHandle wsi, const char *message, int len, MsgHead::Type type)
{
	int err = len;

	MsgHead head;
	head.type_ = type;
	head.len_ = LWS_PRE + len;

	
	send_mutex_.lock();

	auto it = send_cache_.find(wsi);
	if (it != send_cache_.end())
	{
		std::string &sendBuf = it->second.sendBuf_;
		if (sendBuf.size() + len + LWS_PRE > SERVRE_CACHE_MAX)
		{
			len = 0;
			type = MsgHead::Close;
			head.type_ = type;
			head.len_ = LWS_PRE + len;
			sendBuf.clear();
		}
		int oldLen = sendBuf.size();
		if (oldLen <= 0)
			sendBuf.reserve(64 * 1024);
		sendBuf.resize(oldLen + sizeof(head) + head.len_);
		memcpy(&(sendBuf[oldLen]), &head, sizeof head);

		if(len > 0)
			memcpy(&(sendBuf[oldLen + sizeof(head) + LWS_PRE]), message, len);

		if(!it->second.sending_)
			it->second.updated_ = true;
		server_updated_ = true;
	}
	else
	{
		err = -1;
	}
	send_mutex_.unlock();

//  	if(ok)
//  		lws_callback_on_writable((lws*)wsi);

	return err;
}

void WebsocketServer::CloseConnection(WSHandle wsi)
{
	MetaSend(wsi, 0, 0, MsgHead::Close);
}

int WebsocketServer::SendText(WSHandle wsi, const char *message, int len)
{
	if (len <= 0)
		return len;

	return MetaSend(wsi, message, len, MsgHead::Text);
}

int WebsocketServer::SendBinary(WSHandle wsi, const char *message, int len)
{
	if (len <= 0)
		return len;

	return MetaSend(wsi, message, len, MsgHead::Binary);
}

int WebsocketServer::Ping(WSHandle wsi, const char *payload, int len)
{
	if (len < 0)
		return -1;

	return MetaSend(wsi, payload, len, MsgHead::Ping);
}

void WebsocketServer::NewConnection(WSHandle wsi, const char * ip, int port)
{
	LOG4CXX_DEBUG(logAppender,  LOG_VAR(wsi) LOG_VAR(ip) LOG_VAR(port));
	if (m_callBack != nullptr)
	{
		m_callBack->NewConnection(wsi);
	}
}

void WebsocketServer::Disconnection(WSHandle wsi, const char * ip, int port)
{
	LOG4CXX_DEBUG(logAppender, LOG_VAR(wsi) LOG_VAR(ip) LOG_VAR(port));
	if (m_callBack != nullptr)
	{
		m_callBack->Disconnection(wsi);
	}
}

void WebsocketServer::TextReceived(WSHandle wsi, const char * message, int len)
{
	if (m_callBack != nullptr)
	{
		m_callBack->TextReceived(wsi, message, len);
	}
}

void WebsocketServer::BinaryReceived(WSHandle wsi, const char * message, int len)
{
	if (m_callBack != nullptr)
	{
		m_callBack->BinaryReceived(wsi, message, len);
	}
}

void * WebsocketServer::MetaData() const
{
	return meta_data_;
}


