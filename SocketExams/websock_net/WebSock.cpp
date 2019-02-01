#include "stdafx.h"
#include "WebSock.h"
#include <stdio.h>
#include <assert.h>
#include <libwebsockets.h>
#include  <log4cxx/log4cxx.h>
#include <log4cxx/logger.h>
#include "WebsocketClient.h"

LOGGER(logAppender, "WebSock");

extern void ClearRegisterCache(void *wsi);

WebSock::WebSock(WebsocketClient *client_context) : client_context_(client_context),
	auto_reconnect_(false), interval_(3), bFirst_(true)
{

}

WebSock::~WebSock()
{
	if (wsi_)
		ClearRegisterCache(wsi_);
}

void WebSock::SetReconnectInterval(int secs)
{
	if (secs > 0)
		interval_ = secs;
}

void WebSock::SetAutoReconnect(bool enable /*= true*/)
{
	auto_reconnect_ = enable;
}

bool WebSock::IsAutoReconnect() const
{
	return auto_reconnect_;
}

int WebSock::Interval() const
{
	return interval_;
}

void WebSock::Connect(const char *host, int port)
{
	strncpy_s(host_, host, sizeof(host_) - 1);
	port_ = port;
	client_context_->Connect(this, host, port);
}

void WebSock::Close()
{
	bFirst_ = true;
	if (client_context_ && wsi_)
		client_context_->CloseConnection(wsi_);
}

int WebSock::SendText(const char *message, int len)
{
	if (client_context_ && wsi_)
		return client_context_->SendText(wsi_, message, len);

	return -1;
}

int WebSock::SendBinary(const char *message, int len)
{
	if (client_context_ && wsi_)
		return client_context_->SendBinary(wsi_, message, len);

	return -1;
}

int WebSock::Ping(const char *payload, int len)
{
	if (client_context_ && wsi_)
		return client_context_->Ping(wsi_, payload, len);

	return -1;
}

const char * WebSock::Host() const
{
	return host_;
}

int WebSock::Port() const
{
	return port_;
}

void WebSock::SetMetaData(void *wsi)
{
	wsi_ = wsi;
}

bool WebSock::First() const
{
	return bFirst_;
}

void WebSock::__connected(bool ok)
{
	if (ok && bFirst_)
		bFirst_ = false;

	Connected(ok);
}
