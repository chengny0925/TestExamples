#include <iostream>
#include <string>
#include "curl/curl.h"
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "libcurld.lib")

// reply of the requery
size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream)
{
	cout << "----->reply" << endl;
	string *str = (string*)stream;
	cout << *str << endl;
	(*str).append((char*)ptr, size*nmemb);
	return size * nmemb;
}

// http GET
CURLcode curl_get_req(const std::string &url, std::string &response)
{
	// init curl
	CURL *curl = curl_easy_init();
	// res code
	CURLcode res;
	if (curl)
	{
		// set params
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3); // set transport and time out time
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		// start req
		res = curl_easy_perform(curl);
	}
	// release curl
	curl_easy_cleanup(curl);
	return res;
}

// http POST
CURLcode curl_post_req(const string &url, const string &postParams, string &response)
{
	// init curl
	CURL *curl = curl_easy_init();
	// res code
	CURLcode res;
	if (curl)
	{
		// set params
		curl_easy_setopt(curl, CURLOPT_POST, 1); // post req
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str()); // params
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		// start req
		res = curl_easy_perform(curl);
	}
	// release curl
	curl_easy_cleanup(curl);
	return res;
}

int main()
{
	// global init
	curl_global_init(CURL_GLOBAL_ALL);

	// test get requery
	string getUrlStr = "http://cn.bing.com/images/trending?form=Z9LH";
	string getResponseStr;
	auto res = curl_get_req(getUrlStr, getResponseStr);
	if (res != CURLE_OK)
		cerr << "curl_easy_perform() failed: " + string(curl_easy_strerror(res)) << endl;
	else
		cout << getResponseStr << endl;

	// test post requery
	/*{
		string postUrlStr = "https://www.baidu.com/s";
		string postParams = "f=8&rsv_bp=1&rsv_idx=1&word=picture&tn=98633779_hao_pg";
		string postResponseStr;
		auto res = curl_post_req(postUrlStr, postParams, postResponseStr);
		if (res != CURLE_OK)
			cerr << "curl_easy_perform() failed: " + string(curl_easy_strerror(res)) << endl;
		else
			cout << postResponseStr << endl;
	}*/


	// global release
	curl_global_cleanup();
	system("pause");
	return 0;
}
