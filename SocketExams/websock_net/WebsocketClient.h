#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include "websock_net_global.h"
#include "WebsockThread.h"

class WebSock;

class WEBSOCK_NET_CLASS WebsocketClient : public WebsockThread
{
public:
	typedef void *WSHandle;

	WebsocketClient(ThreadPriority prior = Normal, bool enableAutoReconnect = true);
	virtual ~WebsocketClient();

	//  ֻ������֮ǰ����
	void SetReconnectInterval(int secs);
	//  ֻ������֮ǰ����
	void SetAutoReconnect(bool enable = true);

	void Connect(WebSock *websock, const char *host, int port);
	void CloseConnection(WSHandle wsi);                       //  �����ر�����
	int  SendText(WSHandle wsi, const char *message, int len);     //  �����ı�
	int  SendBinary(WSHandle wsi, const char *message, int len);   //  ������������ 
	int  Ping(WSHandle wsi, const char *payload = 0, int len = 0);

	void *MetaData() const;

protected:
	virtual int run() override;      //  �̺߳���
private:
	bool auto_reconnect_;
	int  reconnect_interval_;
	void *meta_data_;
};


#endif  // WEBSOCKET_CLIENT_H