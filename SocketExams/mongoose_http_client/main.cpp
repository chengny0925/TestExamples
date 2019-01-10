#include "mongoose.h"

//static const char *url = "http://www.baidu.com";
//static const char *url = "127.0.0.1:8000/form_submit/";
static const char *url = "127.0.0.1:8000/foo/bar?param1=val1&param2=val2";
static int exit_flag = 0;

static void ev_handler(struct mg_connection *c, int ev, void *p) {
	if (ev == MG_EV_HTTP_REPLY) {
		struct http_message *hm = (struct http_message *)p;
		c->flags |= MG_F_CLOSE_IMMEDIATELY;
		fwrite(hm->message.p, 1, (int)hm->message.len, stdout);
		putchar('\n');
		exit_flag = 1;
	}
	else if (ev == MG_EV_CLOSE) {
		exit_flag = 1;
	};
}

void test()
{
	//struct mg_mgr mgr;

	//mg_mgr_init(&mgr, NULL);

	//mg_connect_http(&mgr, ev_handler, url, NULL, NULL);

	////char data[100] = "test request data";
	////mg_connect_http(&mgr, ev_handler, url, NULL, data);


	//while (exit_flag == 0) {
	//	mg_mgr_poll(&mgr, 1000);
	//}
	//mg_mgr_free(&mgr);
}

#include "HttpClient.h"
#include <iostream>
void test_2()
{
	HttpClient client;
	HttpRsp rsp = client.Get(url);
	if (rsp.error == -1)
	{
		std::cout << "connect error" << endl;
	}
	else if (rsp.error == -2)
	{
		std::cout << "time out" << endl;
	}
	else
	{
		std::cout << "get rsp:\n" << rsp.rsp << endl;
	}

}

int main(void) {

	//test();

	test_2();

	return 0;
}