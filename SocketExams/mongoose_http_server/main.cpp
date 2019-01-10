#include "mongoose.h"
#include <string>
#include <iostream>

static const char *s_http_port = "8000";
std::string rply = "test reply";

static void ev_handler(struct mg_connection *c, int ev, void *p) {
	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message *hm = (struct http_message *) p;

		// We have received an HTTP request. Parsed request is contained in `hm`.
		// Send HTTP reply to the client which shows full original request.
		mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
		mg_printf(c, "%.*s", (int)hm->message.len, hm->message.p);
		mg_printf(c, "%.*s", rply.length(), rply.c_str());
		c->flags |= MG_F_SEND_AND_CLOSE;

		std::string req_str = std::string(hm->message.p, hm->message.len);
		printf("got request: %s\n", req_str.c_str());

		std::string proto = std::string(hm->proto.p, hm->proto.len);
		std::string url = std::string(hm->uri.p, hm->uri.len);
		std::string method = std::string(hm->method.p, hm->method.len);
		std::string qryStr = std::string(hm->query_string.p, hm->query_string.len);
		std::string body = std::string(hm->body.p, hm->body.len);
		printf("proto: %s\n", proto.c_str());
		printf("url: %s\n", url.c_str());
		printf("method: %s\n", method.c_str());
		printf("qryStr: %s\n", qryStr.c_str());
		printf("body: %s\n", body.c_str());

	}
}

int main(void) {
	struct mg_mgr mgr;
	struct mg_connection *c;

	mg_mgr_init(&mgr, NULL);
	c = mg_bind(&mgr, s_http_port, ev_handler);
	if (c == NULL)
	{
		std::cout << "create server error " << std::endl;
		return -1;
	}
	mg_set_protocol_http_websocket(c);

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return 0;
}