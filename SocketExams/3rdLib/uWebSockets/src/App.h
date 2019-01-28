/*
 * Authored by Alex Hultman, 2018-2019.
 * Intellectual property of third-party.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef APP_H
#define APP_H

/* An app is a convenience wrapper of some of the most used fuctionalities and allows a
 * builder-pattern kind of init. Apps operate on the implicit thread local Loop */

#include "HttpContext.h"
#include "HttpResponse.h"
#include "WebSocketContext.h"
#include "WebSocket.h"
#include "WebSocketExtensions.h"

#include "libwshandshake.hpp"

namespace uWS {

/* Compress options (really more like PerMessageDeflateOptions) */
enum CompressOptions {
    /* Compression disabled */
    DISABLED = 0,
    /* We compress using a shared non-sliding window. No added memory usage, worse compression. */
    SHARED_COMPRESSOR = 1,
    /* We compress using a dedicated sliding window. Major memory usage added, better compression of similarly repeated messages. */
    DEDICATED_COMPRESSOR = 2
};

template <bool SSL>
struct TemplatedApp : StaticDispatch<SSL> {
private:
    /* The app always owns at least one http context, but creates websocket contexts on demand */
    HttpContext<SSL> *httpContext;
    std::vector<WebSocketContext<SSL, true> *> webSocketContexts;

    using SOCKET_TYPE = typename StaticDispatch<SSL>::SOCKET_TYPE;
    using StaticDispatch<SSL>::static_dispatch;
public:

    /* Attaches a "filter" function to track socket connections/disconnections */
    void filter(fu2::unique_function<void(HttpResponse<SSL> *, int)> &&filterHandler) {
        httpContext->filter(std::move(filterHandler));
    }

    void registerTag() {

    }

    ~TemplatedApp() {
        /* Let's just put everything here */
        if (httpContext) {
            httpContext->free();

            for (auto *webSocketContext : webSocketContexts) {
                webSocketContext->free();
            }
        }
    }

    /* Disallow copying, only move */
    TemplatedApp(const TemplatedApp &other) = delete;

    TemplatedApp(TemplatedApp &&other) {
        /* Move HttpContext */
        httpContext = other.httpContext;
        other.httpContext = nullptr;

        /* Move webSocketContexts */
        webSocketContexts = std::move(other.webSocketContexts);
    }

    TemplatedApp(us_ssl_socket_context_options sslOptions = {}) {
        httpContext = uWS::HttpContext<SSL>::create(uWS::Loop::defaultLoop(), &sslOptions);
    }

    struct WebSocketBehavior {
        CompressOptions compression = DISABLED;
        int maxPayloadLength = 16 * 1024;
        int idleTimeout = 120;
        std::function<void(uWS::WebSocket<SSL, true> *, HttpRequest *)> open = nullptr;
        std::function<void(uWS::WebSocket<SSL, true> *, std::string_view, uWS::OpCode)> message = nullptr;
        std::function<void(uWS::WebSocket<SSL, true> *)> drain = nullptr;
        std::function<void(uWS::WebSocket<SSL, true> *)> ping = nullptr;
        std::function<void(uWS::WebSocket<SSL, true> *)> pong = nullptr;
        std::function<void(uWS::WebSocket<SSL, true> *, int, std::string_view)> close = nullptr;
    };

    template <class UserData>
    TemplatedApp &&ws(std::string pattern, WebSocketBehavior &&behavior) {
        /* Every route has its own websocket context with its own behavior and user data type */
        auto *webSocketContext = WebSocketContext<SSL, true>::create(Loop::defaultLoop(), (typename StaticDispatch<SSL>::SOCKET_CONTEXT_TYPE *) httpContext);

        /* We need to clear this later on */
        webSocketContexts.push_back(webSocketContext);

        /* Quick fix to disable any compression if set */
#ifdef UWS_NO_ZLIB
        behavior.compression = uWS::DISABLED;
#endif

        /* If we are the first one to use compression, initialize it */
        if (behavior.compression) {
            LoopData *loopData = (LoopData *) us_loop_ext(static_dispatch(us_ssl_socket_context_loop, us_socket_context_loop)(webSocketContext->getSocketContext()));

            /* Initialize loop's deflate inflate streams */
            if (!loopData->zlibContext) {
                loopData->zlibContext = new ZlibContext;
                loopData->inflationStream = new InflationStream;
                loopData->deflationStream = new DeflationStream;
            }
        }

        /* Copy all handlers */
        webSocketContext->getExt()->messageHandler = behavior.message;
        webSocketContext->getExt()->drainHandler = behavior.drain;
        webSocketContext->getExt()->closeHandler = behavior.close;

        /* Copy settings */
        webSocketContext->getExt()->maxPayloadLength = behavior.maxPayloadLength;
        webSocketContext->getExt()->idleTimeout = behavior.idleTimeout;

        return std::move(get(pattern, [webSocketContext, this, behavior](auto *res, auto *req) {
            /* If we have this header set, it's a websocket */
            std::string_view secWebSocketKey = req->getHeader("sec-websocket-key");
            if (secWebSocketKey.length()) {
                // note: OpenSSL can be used here to speed this up somewhat
                char secWebSocketAccept[29] = {};
                WebSocketHandshake::generate(secWebSocketKey.data(), secWebSocketAccept);

                res->writeStatus("101 Switching Protocols")
                    ->writeHeader("Upgrade", "websocket")
                    ->writeHeader("Connection", "Upgrade")
                    ->writeHeader("Sec-WebSocket-Accept", secWebSocketAccept);

                /* Negotiate compression */
                bool perMessageDeflate = false;
                bool slidingDeflateWindow = false;
                if (behavior.compression != DISABLED) {
                    std::string_view extensions = req->getHeader("sec-websocket-extensions");
                    if (extensions.length()) {
                        /* We never support client context takeover (the client cannot compress with a sliding window). */
                        int wantedOptions = PERMESSAGE_DEFLATE | CLIENT_NO_CONTEXT_TAKEOVER;

                        /* Shared compressor is the default */
                        if (behavior.compression == SHARED_COMPRESSOR) {
                            /* Disable per-socket compressor */
                            wantedOptions |= SERVER_NO_CONTEXT_TAKEOVER;
                        }

                        /* isServer = true */
                        ExtensionsNegotiator<true> extensionsNegotiator(wantedOptions);
                        extensionsNegotiator.readOffer(extensions);

                        /* Todo: remove these mid string copies */
                        res->writeHeader("Sec-WebSocket-Extensions", extensionsNegotiator.generateOffer());

                        /* Did we negotiate permessage-deflate? */
                        if (extensionsNegotiator.getNegotiatedOptions() & PERMESSAGE_DEFLATE) {
                            perMessageDeflate = true;
                        }

                        /* Is the server allowed to compress with a sliding window? */
                        if (!(extensionsNegotiator.getNegotiatedOptions() & SERVER_NO_CONTEXT_TAKEOVER)) {
                            slidingDeflateWindow = true;
                        }
                    }
                }

                /* Add mark, we don't want to end anything */
                res->writeHeader("WebSocket-Server", "uWebSockets")->end();

                /* bug: memory leak? What about HttpResponseData here? I'm thinking not delete it but destruct it? */
                /* Esp. if the functions hold dynamic memory like something big */

                /* Vad om vi inte får plats i cork, och vi har vårt svar i corkbuffern */

                /* vad om vi är en socket som har endat, men har svaret i sin buffer och strömmas ut? */

                /* vi antar att vi kan bara kasta asyncsocketdata här */

                /* res.end ska egentligen inte sätta att man svarat på den före den har drainats */

                /* Adopting a socket invalidates it, do not rely on it directly to carry any data */
                WebSocket<SSL, true> *webSocket = (WebSocket<SSL, true> *) StaticDispatch<SSL>::static_dispatch(us_ssl_socket_context_adopt_socket, us_socket_context_adopt_socket)(
                            (typename StaticDispatch<SSL>::SOCKET_CONTEXT_TYPE *) webSocketContext, (typename StaticDispatch<SSL>::SOCKET_TYPE *) res, sizeof(WebSocketData) + sizeof(UserData));

                /* Update corked socket in case we got a new one (assuming we always are corked in handlers). */
                webSocket->cork();

                httpContext->upgradeToWebSocket(
                            webSocket->init(perMessageDeflate, slidingDeflateWindow)
                            );

                /* Emit open event and start the timeout */
                if (behavior.open) {
                    static_dispatch(us_ssl_socket_timeout, us_socket_timeout)((SOCKET_TYPE *) webSocket, behavior.idleTimeout);
                    behavior.open(webSocket, req);
                }

                /* We are going to get uncorked by the Http get return */

                /* We do not need to check for any close or shutdown here as we immediately return from get handler */

            } else {
                /* Tell the router that we did not handle this request */
                req->setYield(true);
            }
        }));
    }

    TemplatedApp &&get(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("get", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&post(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("post", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&options(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("options", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&del(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("delete", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&patch(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("patch", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&put(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("put", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&head(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("head", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&connect(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("connect", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&trace(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("trace", pattern, handler);
        return std::move(*this);
    }

    /* This one catches any method */
    TemplatedApp &&any(std::string pattern, std::function<void(HttpResponse<SSL> *, HttpRequest *)> handler) {
        httpContext->onHttp("*", pattern, handler);
        return std::move(*this);
    }

    TemplatedApp &&listen(int port, std::function<void(us_listen_socket *)> handler) {
        handler(httpContext->listen(nullptr, port, 0));
        return std::move(*this);
    }

    TemplatedApp &&run() {
        uWS::run();
        return std::move(*this);
    }

};

typedef TemplatedApp<false> App;
typedef TemplatedApp<true> SSLApp;

}

#endif // APP_H
