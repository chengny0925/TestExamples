#ifndef WEBSOCK_H
#define WEBSOCK_H

#include "websock_net_global.h"
#include <stdint.h>

class WebsocketClient;

class  WEBSOCK_NET_CLASS WebSock
{
public:
	WebSock(WebsocketClient *client_context);
	virtual ~WebSock();

	void SetReconnectInterval(int secs);
	void SetAutoReconnect(bool enable = true);
	bool IsAutoReconnect() const;
	int  Interval() const;

	void Connect(const char *host, int port);
	void Close();
	int  SendText(const char *message, int len);
	int  SendBinary(const char *message, int len);
	int  Ping(const char *payload, int len);

	const char *Host() const;
	int Port() const;
	////////////////////
	void SetMetaData(void *wsi);
	bool First() const;
	void __connected(bool ok);
public:
	virtual void Connected(bool ok) {}    //  连接返回
	virtual void Disconnected() {}        //  掉线返回
	virtual void TextReceived(const char *message, int len) {}
	virtual void BinaryReceived(const char *message, int len) {}
	virtual void TextFrameReceived(const char *message, int len, bool isLast) {}
	virtual void BinaryFrameReceived(const char *message, int len, bool isLast) {}
	virtual void Pong(uint64_t elapesdMilliSecs, const char *payload, int len) {}
private:
	WebsocketClient *client_context_;
	void *wsi_;
	bool auto_reconnect_;
	int interval_;
	char host_[64];
	int port_;
	bool bFirst_;
};

#endif  // WEBSOCK_H