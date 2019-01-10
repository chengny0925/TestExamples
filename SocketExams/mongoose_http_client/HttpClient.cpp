#include "HttpClient.h"
#include <functional>
#include <chrono>

HttpClient::HttpClient()
{
}


HttpClient::~HttpClient()
{
}

HttpRsp HttpClient::Get(string url, int timeOut)
{
	m_rsp = {};
	struct mg_mgr mgr;

	mg_mgr_init(&mgr, NULL);
	UINT64 begin = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

	//mg_connection * connection = mg_connect_http(&mgr, ev_handler, (void *)this, url.c_str(), NULL, NULL);

	mg_connection * connection = mg_connect_http(&mgr, ev_handler, url.c_str(), NULL, NULL);
	connection->user_data = (void *)this;
	mg_set_protocol_http_websocket(connection);


	//char data[100] = "test request data";
	//mg_connect_http(&mgr, ev_handler, url, NULL, data);


	while (m_exitFlag == 0)
	{
		UINT64 end = std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();
		if (end - begin > timeOut * 1e6)
		{
			m_rsp.error = HttpError::TimeOut;
			break;
		}
		mg_mgr_poll(&mgr, 500);
	}
	mg_mgr_free(&mgr);


	return m_rsp;
}


//void HttpClient::ev_handler(mg_connection * connection, int event_type, void *event_data, void *user_data)
//{
//	http_message *hm = (struct http_message *)event_data;
//	HttpClient* pClient = (HttpClient*)user_data;
//
//	switch (event_type)
//	{
//	case MG_EV_CONNECT:
//	{
//		int connect_status;
//		connect_status = *(int *)event_data;
//		if (connect_status != 0)
//		{
//			printf("Error connecting to server, error code: %d\n", connect_status);
//			pClient->m_exitFlag = 1;
//		}
//		break;
//	}
//	case MG_EV_HTTP_REPLY:
//	{
//		printf("Got reply:\n%.*s\n", (int)hm->body.len, hm->body.p);
//		std::string rsp = std::string(hm->body.p, hm->body.len);
//		connection->flags |= MG_F_SEND_AND_CLOSE;
//		pClient->m_exitFlag = 1; // 每次收到请求后关闭本次连接，重置标记
//
//		pClient->m_rsp = rsp;
//		break;
//	}
//
//	case MG_EV_CLOSE:
//	{
//		if (pClient->m_exitFlag == 0)
//		{
//			printf("Server closed connection\n");
//			pClient->m_exitFlag = 1;
//		};
//		break;
//	}
//
//	default:
//		break;
//	}
//
//}


void HttpClient::ev_handler(mg_connection * connection, int event_type, void * event_data)
{
		http_message *hm = (struct http_message *)event_data;
		HttpClient* pClient = (HttpClient*)connection->user_data;
	
		switch (event_type)
		{
		case MG_EV_CONNECT:
		{
			int connect_status;
			connect_status = *(int *)event_data;
			if (connect_status != 0)
			{
				printf("Error connecting to server, error code: %d\n", connect_status);
				pClient->m_exitFlag = 1;
				pClient->m_rsp.error = HttpError::ConnectError;
			}
			break;
		}
		case MG_EV_HTTP_REPLY:
		{
			printf("Got reply:\n%.*s\n", (int)hm->body.len, hm->body.p);
			std::string rsp = std::string(hm->body.p, hm->body.len);
			connection->flags |= MG_F_SEND_AND_CLOSE;
			pClient->m_exitFlag = 1; // 每次收到请求后关闭本次连接，重置标记
	
			pClient->m_rsp.rsp = rsp;
			break;
		}
	
		case MG_EV_CLOSE:
		{
			if (pClient->m_exitFlag == 0)
			{
				printf("Server closed connection\n");
				pClient->m_exitFlag = 1;
			};
			break;
		}
	
		default:
			break;
		}
}


