#include "stdafx.h"
#include "WebsocketClient.h"
#include "WebSock.h"
#include <libwebsockets.h>
#include <mutex>
#include <stdio.h>
#include <assert.h>
#include  <log4cxx/log4cxx.h>
#include <log4cxx/logger.h>
#include <vector>
#include <time.h>
#include <set>

LOGGER(logAppender, "WebsocketClient");

#define SERVRE_CACHE_MAX   16*1024*1024

__declspec(thread) WebsocketClient *this_websock_client_ = 0;
__declspec(thread) static bool bBusy_;

static bool bnew_data_to_send_;

extern int strPeerName(SOCKET s, char *ip, int ipLen, int &port);

struct ClientMsgItem
{
	WebSock *websock_;
	std::string sendBuf_;
	bool updated_;
	bool sending_;
	ClientMsgItem() : websock_(0), updated_(false), sending_(false) {}
};

struct ClientInOut
{
	std::string receive_;
	std::string send_;
	int curSendPos_;
	bool bframe_;
	bool isBinary_;
	WebSock *websock_;
	ClientInOut() : curSendPos_(0) , bframe_(false), isBinary_(false), websock_(0)
	{
		send_.reserve(64 * 1024);
		receive_.reserve(64 * 1024);
	}
};


bool has_new_connect_ = false;

//static std::map<void *, void *> wsi_map_websock_;
static std::map<void *, ClientMsgItem> client_message_cache_;
static std::recursive_mutex client_mutex_;
//__declspec(thread) std::map<void*, ClientInOut> client_inout_;


void ClearRegisterCache(void *wsi)
{
	client_mutex_.lock();
	//wsi_map_websock_.erase(wsi);
	client_message_cache_.erase(wsi);
	client_mutex_.unlock();
}

struct ClientMsgHead
{
	enum Type { Text, Binary, Ping, Close };
	Type type_;
	int len_;
};

struct ConnectItem
{
	WebSock *websock_;
	char host_[64];
	int port_;
	time_t  timestamp_;
	bool first_;
};

struct ClientMetaData
{
	lws_context *context_;
	ClientMetaData() : context_(0) {}
};

struct ClientUserData
{
	void *web_sock_;
};

static std::vector<ConnectItem> connect_queue_;

int check_reconnect(struct lws *wsi, enum lws_callback_reasons reason,
	void *user, void *in, size_t len)
{
	WebSock *websock = 0;
	client_mutex_.lock();

	auto it = client_message_cache_.find(wsi);
	if (it != client_message_cache_.end())
	{
		websock = it->second.websock_;
		client_message_cache_.erase(it);
	}
	//wsi_map_websock_.erase(wsi);

	client_mutex_.unlock();

	if (websock)
	{
		websock->Disconnected();
		assert(this_websock_client_);
		if (websock->IsAutoReconnect() && !websock->First())
		{
			ConnectItem item = { 0 };
			item.websock_ = websock;
			strncpy_s(item.host_, websock->Host(), sizeof(item.host_) - 1);
			item.port_ = websock->Port();
			int val = websock->Interval();
			if (val <= 0)
				val = 1;
			item.timestamp_ = time(0) + val;

			client_mutex_.lock();
			connect_queue_.push_back(item);
			has_new_connect_ = true;
			client_mutex_.unlock();
		}
	}

	return 0;
}

static int client_callback(struct lws *wsi, enum lws_callback_reasons reason,
	void *user, void *in, size_t len)
{
	switch (reason)
	{
	case LWS_CALLBACK_CLOSED:
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
	{
		bBusy_ = true;
		LOG4CXX_INFO(logAppender, LOG_VAR(reason) LOG_VAR(wsi));
		return -1;
		break;
	}
	case LWS_CALLBACK_WSI_CREATE:
	{
		bBusy_ = true;
		break;
	}
	case LWS_CALLBACK_WSI_DESTROY:
	{
		bBusy_ = true;
		ClientInOut * &pinout = *(ClientInOut **)user;

		check_reconnect(wsi, reason, user, in, len);

		if (user && pinout)
			delete pinout;
			
		break;
	}
	case LWS_CALLBACK_CLIENT_ESTABLISHED:
	{
		bBusy_ = true;
		assert(user);
		ClientInOut * &pinout = *(ClientInOut **)user;
		pinout = new (std::nothrow) ClientInOut;
	
		client_mutex_.lock();
		auto it = client_message_cache_.find(wsi);
		if (it != client_message_cache_.end())
		{
			pinout->websock_ = (WebSock*)it->second.websock_;
		}
		client_mutex_.unlock();
		
		if (pinout->websock_)
			pinout->websock_->__connected(true);

		break;
	}
	case LWS_CALLBACK_RECEIVE_PONG:
	{
		bBusy_ = true;
		ClientInOut * &pinout = *(ClientInOut **)user;
		if (pinout)
		{
			auto &inout = *pinout;
			uint64_t timeElapsed = 0;
			if (!inout.websock_)
			{
				client_mutex_.lock();
				auto it = client_message_cache_.find(wsi);
				if (it != client_message_cache_.end())
				{
					inout.websock_ = (WebSock*)it->second.websock_;
				}
				client_mutex_.unlock();
			}

			if (inout.websock_)
			{
				inout.websock_->Pong(timeElapsed, (char*)in, len);
			}
		}
		
		break;
	}
	case LWS_CALLBACK_CLIENT_RECEIVE:
	{
		bBusy_ = true;
		ClientInOut * &pinout = *(ClientInOut **)user;
		if (pinout)
		{
			auto &inout = *pinout;
			if (!inout.websock_)
			{
				client_mutex_.lock();
				auto it = client_message_cache_.find(wsi);
				if (it != client_message_cache_.end())
				{
					inout.websock_ = (WebSock*)it->second.websock_;
				}
				client_mutex_.unlock();
			}

			if (inout.websock_)
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

				if (0 == remaining)
				{
					if (!inout.bframe_)
					{
						if (isBinary)
							inout.websock_->BinaryReceived(inout.receive_.data(), inout.receive_.length());
						else
							inout.websock_->TextReceived(inout.receive_.data(), inout.receive_.length());
					}
					else
					{
						if (inout.isBinary_)
							inout.websock_->BinaryFrameReceived((char*)in, len, 0 != isFinal);
						else
							inout.websock_->TextFrameReceived((char*)in, len, 0 != isFinal);
					}

					inout.receive_.resize(0);
					inout.bframe_ = false;
				}
			}
		}
		
		break;
	}
	case LWS_CALLBACK_CLIENT_WRITEABLE:
	{
		bBusy_ = true;
		ClientInOut * &pinout = *(ClientInOut **)user;
		if (pinout)
		{
			auto &inout = *pinout;
			bool exist = true;
			if (inout.send_.size() < inout.curSendPos_ + sizeof(ClientMsgHead))
			{
				inout.curSendPos_ = 0;
				inout.send_.resize(0);
				client_mutex_.lock();

				auto it = client_message_cache_.find(wsi);
				if (it != client_message_cache_.end())
				{
					it->second.sendBuf_.swap(inout.send_);
					if (inout.send_.size() <= 0)
					{
						it->second.sending_ = false;
						exist = false;
					}
					else
						it->second.sending_ = true;
				}
				else
					exist = false;

				client_mutex_.unlock();

				if (inout.send_.size() > SERVRE_CACHE_MAX)
				{
					inout.send_.clear();
					return -1;
				}
			}

			if (!exist)
				return 0;

			try
			{
				while (inout.send_.size() >= sizeof(ClientMsgHead) + inout.curSendPos_)
				{
					ClientMsgHead *pHead = (ClientMsgHead*)&inout.send_[inout.curSendPos_];
					bool closed = false;
					switch (pHead->type_)
					{
					case ClientMsgHead::Close:
					{
						return -1;
						SOCKET sockfd = lws_get_socket_fd(wsi);
						int err = ::closesocket(sockfd);
						if (err)
						{
							LOG4CXX_ERROR(logAppender, "closesocket()" LOG_VAR(__LINE__) LOG_VAR(err) LOG_VAR(sockfd));
						}
						closed = true;
						break;
					}
					case ClientMsgHead::Text:
					{
						assert(pHead->len_ >= LWS_PRE);
						lws_write(wsi, (unsigned char*)&inout.send_[inout.curSendPos_ + sizeof(ClientMsgHead) + LWS_PRE], pHead->len_ - LWS_PRE, LWS_WRITE_TEXT);
						//lws_callback_on_writable(wsi);
						break;
					}
					case ClientMsgHead::Binary:
					{
						assert(pHead->len_ >= LWS_PRE);
						lws_write(wsi, (unsigned char*)&inout.send_[inout.curSendPos_ + sizeof(ClientMsgHead) + LWS_PRE], pHead->len_ - LWS_PRE, LWS_WRITE_BINARY);
						//lws_callback_on_writable(wsi);
						break;
					}
					case ClientMsgHead::Ping:
					{
						assert(pHead->len_ >= LWS_PRE);
						lws_write(wsi, (unsigned char*)&inout.send_[inout.curSendPos_ + sizeof(ClientMsgHead) + LWS_PRE], pHead->len_ - LWS_PRE, LWS_WRITE_PING);
						//lws_callback_on_writable(wsi);
						break;
					}

					default:
						assert(0);
						break;
					}

					inout.curSendPos_ += sizeof(ClientMsgHead) + pHead->len_;
					if (inout.curSendPos_ >= inout.send_.size())
					{
						inout.curSendPos_ = 0;
						inout.send_.resize(0);
						if (lws_partial_buffered(wsi) != 1)
							lws_callback_on_writable(wsi);
						break;
					}

					if(lws_partial_buffered(wsi) == 1)
						break;
				}
			}
			catch (...)
			{
				LOG4CXX_FATAL(logAppender, "LWS_CALLBACK_CLIENT_WRITEABLE catch an exception" LOG_VAR(__LINE__));
			}
		}
		
		break;
	}
	}
	return 0;
}

static struct lws_protocols client_protocols[] =
{
	{
		"client-protocol", // protocol name - very important!
		client_callback,   // callback
		sizeof(void*)
	},
	{
		NULL, NULL, 0   /* End of list */
	}
};

static int ClinetMetaSend(WebsocketClient::WSHandle wsi, const char *message, int len, ClientMsgHead::Type type)
{
	if (!wsi)
		return -1;

	ClientMsgHead head;
	head.type_ = type;
	head.len_ = len + LWS_PRE;

	client_mutex_.lock();

	auto &item = client_message_cache_[wsi];
	std::string &sendBuf = item.sendBuf_;
	if (sendBuf.capacity() < 64 * 1024)
		sendBuf.reserve(64 * 1024);

	if (sendBuf.size() + len + LWS_PRE > SERVRE_CACHE_MAX)
	{
		len = 0;
		type = ClientMsgHead::Close;
		head.type_ = type;
		head.len_ = len + LWS_PRE;
		sendBuf.clear();
	}

	auto oldBeginPos = sendBuf.size();
	sendBuf.resize(oldBeginPos + sizeof(head) + head.len_);
	memcpy(&sendBuf[oldBeginPos], &head, sizeof(head));
	if (len > 0)
		memcpy(&sendBuf[oldBeginPos + sizeof(head) + LWS_PRE], message, len);

	if (!item.sending_)
	{
		item.updated_ = true;
		bnew_data_to_send_ = true;
	}
	
	client_mutex_.unlock();

	return 0;
}

WebsocketClient::WebsocketClient(ThreadPriority prior /*= Normal*/, bool enableAutoReconnect /*= true*/)
	: WebsockThread(prior), auto_reconnect_(enableAutoReconnect), reconnect_interval_(3), meta_data_(new ClientMetaData)
{

}

WebsocketClient::~WebsocketClient()
{
	Stop();
	if (meta_data_)
		delete (ClientMetaData*)meta_data_;
}

void WebsocketClient::SetReconnectInterval(int secs)
{
	if (secs > 0)
		reconnect_interval_ = secs;
}

void WebsocketClient::SetAutoReconnect(bool enable /*= true*/)
{
	auto_reconnect_ = enable;
}

void WebsocketClient::Connect(WebSock *websock, const char *host, int port)
{
	if (!websock)
		return;

	ConnectItem item = { 0 };
	item.websock_ = websock;
	strncpy_s(item.host_, host, sizeof(item.host_) - 1);
	item.port_ = port;
	item.first_ = true;

	client_mutex_.lock();
	connect_queue_.push_back(item);
	has_new_connect_ = true;
	client_mutex_.unlock();
}

void WebsocketClient::CloseConnection(WSHandle wsi)
{
	ClinetMetaSend(wsi, 0, 0, ClientMsgHead::Close);
}

int WebsocketClient::SendText(WSHandle wsi, const char *message, int len)
{
	if (len <= 0)
		return -1;
	return ClinetMetaSend(wsi, message, len, ClientMsgHead::Text);
}

int WebsocketClient::SendBinary(WSHandle wsi, const char *message, int len)
{
	if (len <= 0)
		return -1;
	return ClinetMetaSend(wsi, message, len, ClientMsgHead::Binary);
}

int WebsocketClient::Ping(WSHandle wsi, const char *payload /*= 0*/, int len /*= 0*/)
{
	if (len <= 0)
		return -1;
	return ClinetMetaSend(wsi, payload, len, ClientMsgHead::Ping);
}

void * WebsocketClient::MetaData() const
{
	return meta_data_;
}



int WebsocketClient::run()
{
	this_websock_client_ = this;

	struct lws_context_creation_info info = { 0 };
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = client_protocols;
	info.gid = -1;
	info.uid = -1;
	info.timeout_secs = 30;
	auto_reconnect_ = false;
	if (auto_reconnect_)
	{
		info.ka_time = 1;
		info.ka_probes = 1;
	}
	info.ka_interval = reconnect_interval_;

	lws_context * context = lws_create_context(&info);
	if (!context)
	{
		LOG4CXX_FATAL(logAppender, "run() lws_create_context() failed");
		return -1;
	}
		
	ClientMetaData *pmeta = (ClientMetaData *)meta_data_;
	pmeta->context_ = context;

	std::vector<ConnectItem> delayed_connect;

	std::vector<lws*> updateds;
	updateds.reserve(1024);

	while (!stopped_)
	{
		time_t nowtime = time(0);
		for (auto it = delayed_connect.begin(); it != delayed_connect.end(); )
		{
			if (it->timestamp_ < nowtime)
			{
				if ((*it).websock_)
				{
					lws_client_connect_info c_info = { 0 };
					c_info.address = (*it).host_;
					c_info.port = (*it).port_;
					c_info.context = context;
					c_info.path = "/";
					char buf[64];
					_snprintf_s(buf, sizeof(buf) - 1, "%s:%d", c_info.address, c_info.port);
					c_info.host = c_info.origin = buf;

					LOG4CXX_INFO(logAppender, "自动重连 " LOG_VAR((*it).websock_) LOG_VAR(c_info.host));

					lws *wsi = lws_client_connect_via_info(&c_info);
					if (!wsi)
					{
						(*it).websock_->Connected(false);
						if ((*it).websock_->IsAutoReconnect() && !(*it).websock_->First())
						{
							ConnectItem tmpItem = (*it);
							tmpItem.first_ = false;
							int val = (*it).websock_->Interval();
							if (val <= 0)
								val = 1;
							tmpItem.timestamp_ = time(0) + val;

							client_mutex_.lock();
							connect_queue_.push_back(tmpItem);
							client_mutex_.unlock();
						}
					}
					else
					{
						(*it).websock_->SetMetaData(wsi);
						client_mutex_.lock();
						//wsi_map_websock_[wsi] = (*it).websock_;
						client_message_cache_[wsi].websock_ = (*it).websock_;
						client_mutex_.unlock();
					}
				}

				it = delayed_connect.erase(it);
				continue;
			}
			else
				++it;
		}

		if (has_new_connect_)
		{
			std::vector<ConnectItem> tmp_connect;

			client_mutex_.lock();
			tmp_connect.swap(connect_queue_);
			has_new_connect_ = false;
			client_mutex_.unlock();

			for (auto i = 0; i < tmp_connect.size(); ++i)
			{
				if (tmp_connect[i].timestamp_ >= nowtime)
				{
					delayed_connect.push_back(tmp_connect[i]);
					continue;
				}
				if (tmp_connect[i].websock_)
				{
					lws_client_connect_info c_info = { 0 };
					c_info.address = tmp_connect[i].host_;
					c_info.port = tmp_connect[i].port_;
					c_info.context = context;
					c_info.path = "/";
					char buf[64];
					_snprintf_s(buf, sizeof(buf) - 1, "%s:%d", c_info.address, c_info.port);
					c_info.host = c_info.origin = buf;
					lws *wsi = lws_client_connect_via_info(&c_info);
					if (!wsi)
					{
						tmp_connect[i].websock_->Connected(false);
						if (tmp_connect[i].websock_->IsAutoReconnect() && !tmp_connect[i].websock_->First())
						{
							ConnectItem tmpItem = tmp_connect[i];
							tmpItem.first_ = false;
							int val = tmp_connect[i].websock_->Interval();
							if (val <= 0)
								val = 1;
							tmpItem.timestamp_ = time(0) + val;
							
							client_mutex_.lock();
							connect_queue_.push_back(tmpItem);
							client_mutex_.unlock();
						}
					}
					else
					{
						tmp_connect[i].websock_->SetMetaData(wsi);
						client_mutex_.lock();
						//wsi_map_websock_[wsi] = tmp_connect[i].websock_;
						client_message_cache_[wsi].websock_ = tmp_connect[i].websock_;
						client_mutex_.unlock();
					}
				}
			}
		}

		if (bnew_data_to_send_)
		{
			updateds.clear();

			client_mutex_.lock();
			for (auto &item1 : client_message_cache_)
			{
				if (item1.second.updated_)
				{
					item1.second.updated_ = false;
					updateds.push_back((lws*)item1.first);
				}
			}
			bnew_data_to_send_ = false;
			client_mutex_.unlock();

			for (auto &item : updateds)
			{
				if (item)
					lws_callback_on_writable(item);
			}
		}
		
		do 
		{
			bBusy_ = false;
			lws_service(context, 0);
		} while (bBusy_);

		if (!has_new_connect_ && !bnew_data_to_send_)
			::Sleep(1);
	}
		

	return 0;
}
