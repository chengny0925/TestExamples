#include "stdafx.h"
#include "WebsocketServerApi.h"
#include "WebsocketServer.h"

WEBSOCK_NET_FUN IWebsocketServer * CreateWebsocketServer()
{
	return new WebsocketServer();
}
