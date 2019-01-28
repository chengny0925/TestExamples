//#include "App.h"
//
//int main() {
//	/* Overly simple hello world app */
//	uWS::App().get("/*", [](auto *res, auto *req) {
//		res->end("Hello world!");
//	}).listen(3000, [](auto *token) {
//		if (token) {
//			std::cout << "Listening on port " << 3000 << std::endl;
//		}
//	}).run();
//
//	std::cout << "Failed to listen on port 3000" << std::endl;
//}

#include "App.h"

int main() {
	/* ws->getUserData returns one of these */
	struct PerSocketData {
		int id;
	};

	/* Very simple WebSocket echo server */
	uWS::App::WebSocketBehavior behavior;
	behavior.compression = uWS::SHARED_COMPRESSOR;
	behavior.maxPayloadLength = 16 * 1024,
		/* Handlers */
		behavior.open = [](auto *ws, auto *req) {
			uWS::WebSocket<false, true> * server = ws;
			uWS::HttpRequest* preq = req;
			std::cout << "behavior.open method=" << preq->getMethod() << std::endl;
			PerSocketData* data = (PerSocketData*)server->getUserData();
			data->id = 101;
		};
		behavior.message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
			std::cout << "behavior.message msg=" << message << std::endl;
			ws->send(message, opCode);
		};
		behavior.drain = [](auto *ws) {
		/* Check getBufferedAmount here */
		};
		behavior.ping = [](auto *ws) {

		};
		behavior.pong = [](auto *ws) {

		};
		behavior.close = [](auto *ws, int code, std::string_view message) {
			uWS::WebSocket<false, true> * server = ws;
			PerSocketData* data = (PerSocketData*)server->getUserData();
			std::cout << "behavior.close code=" <<  code << ", id=" << data->id << std::endl;
		};

	uWS::App().ws<PerSocketData>((std::string)"/*", std::move(behavior)).listen(9001, [](auto *token){
			if (token) {
							std::cout << "Listening on port " << 9001 << std::endl;
					}
		}).run();

	//uWS::App().ws<PerSocketData>("/*", {
	//	/* Settings */
	//	.compression = uWS::SHARED_COMPRESSOR,
	//	.maxPayloadLength = 16 * 1024,
	//	/* Handlers */
	//	.open = [](auto *ws, auto *req) {

	//	},
	//	.message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
	//		ws->send(message, opCode);
	//	},
	//	.drain = [](auto *ws) {
	//		/* Check getBufferedAmount here */
	//	},
	//	.ping = [](auto *ws) {

	//	},
	//	.pong = [](auto *ws) {

	//	},
	//	.close = [](auto *ws, int code, std::string_view message) {

	//	}
	//	}).listen(9001, [](auto *token) {
	//		if (token) {
	//			std::cout << "Listening on port " << 9001 << std::endl;
	//		}
	//	}).run();
}
