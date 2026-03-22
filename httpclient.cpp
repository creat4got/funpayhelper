#include "httpclient.h"

#include <string>
#include <sstream>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace ssl   = net::ssl;
using tcp = net::ip::tcp;

HttpClient::HttpClient() {}

// ─── Generic HTTPS helper ─────────────────────────────────────────────────────
static bool performRequest(
    http::verb  verb,
    const std::string& host,
    const std::string& target,
    const std::string& cookieHeader,
    const std::string& body,
    const std::string& contentType,
    const std::vector<std::pair<std::string,std::string>>& extraHeaders,
    std::string& responseBody,
    std::string& redirectLocation,
    int&         statusCode,
    std::string& errorText,
    std::string* outSetCookie = nullptr)
{
    responseBody.clear();
    redirectLocation.clear();
    errorText.clear();
    statusCode = 0;

    try {
        net::io_context ioc;
        ssl::context sslCtx(ssl::context::tlsv12_client);
        sslCtx.set_default_verify_paths();
        sslCtx.set_verify_mode(ssl::verify_peer);

        tcp::resolver resolver(ioc);
        beast::ssl_stream<beast::tcp_stream> stream(ioc, sslCtx);

        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            beast::error_code ec(static_cast<int>(::ERR_get_error()),
                                 net::error::get_ssl_category());
            errorText = "SNI error: " + ec.message();
            return false;
        }

        beast::get_lowest_layer(stream).connect(resolver.resolve(host, "443"));
        stream.handshake(ssl::stream_base::client);

        http::request<http::string_body> req(verb, target, 11);
        req.set(http::field::host,       host);
        req.set(http::field::user_agent,
                "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                "(KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36");
        req.set(http::field::accept,
                "text/html,application/xhtml+xml,application/xml;q=0.9,"
                "image/avif,image/webp,image/apng,*/*;q=0.8");
        req.set("Accept-Language", "ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7");

        if (!cookieHeader.empty())
            req.set(http::field::cookie, cookieHeader);

        for (const auto& h : extraHeaders)
            req.set(h.first, h.second);

        if (!body.empty()) {
            req.body() = body;
            req.prepare_payload();
            if (!contentType.empty())
                req.set(http::field::content_type, contentType);
        }

        http::write(stream, req);

        beast::flat_buffer buf;
        http::response<http::string_body> resp;
        http::read(stream, buf, resp);

        statusCode   = resp.result_int();
        responseBody = resp.body();

        if (resp.base().find(http::field::location) != resp.base().end())
            redirectLocation = std::string(resp.base()[http::field::location]);

        // Собираем Set-Cookie заголовки из всех ответов в цепочке редиректов
        if (outSetCookie) {
            for (auto const& field : resp.base()) {
                if (beast::iequals(field.name_string(), "set-cookie"))
                    *outSetCookie += std::string(field.value()) + "\n";
            }
        }

        beast::error_code ec;
        stream.shutdown(ec);
        if (ec == net::error::eof || ec == ssl::error::stream_truncated) ec = {};
        if (ec) { errorText = "SSL shutdown: " + ec.message(); return false; }

        return true;
    }
    catch (const std::exception& ex) {
        errorText = ex.what();
        return false;
    }
}

// ─── Redirect loop ────────────────────────────────────────────────────────────
static bool sendWithRedirects(
    http::verb  verb,
    const std::string& host,
    const std::string& target,
    const std::string& cookieHeader,
    const std::string& body,
    const std::string& contentType,
    const std::vector<std::pair<std::string,std::string>>& extraHeaders,
    std::string& responseBody,
    std::string& errorText,
    std::string* outSetCookie = nullptr)
{
    std::string cur = target;

    for (int i = 0; i < 5; ++i) {
        std::string loc;
        int code = 0;

        bool ok = performRequest(verb, host, cur, cookieHeader,
                                 body, contentType, extraHeaders,
                                 responseBody, loc, code, errorText, outSetCookie);
        if (!ok) return false;
        if (code >= 200 && code < 300) return true;

        if (code >= 300 && code < 400) {
            if (loc.empty()) { errorText = "Redirect without Location"; return false; }
            const std::string base = "https://funpay.com";
            if (loc.find(base) == 0) {
                cur = loc.substr(base.size());
                if (cur.empty()) cur = "/";
            } else if (!loc.empty() && loc[0] == '/') {
                cur = loc;
            } else {
                errorText = "Unsupported redirect: " + loc;
                return false;
            }
            verb = http::verb::get; // POST→redirect→GET
            continue;
        }

        // Для 4xx/5xx возвращаем тело ответа (помогает диагностировать ошибки)
        std::ostringstream oss;
        oss << "HTTP error " << code;
        errorText = oss.str();
        // responseBody уже заполнен в performRequest — не очищаем
        return false;
    }

    errorText = "Too many redirects";
    return false;
}

bool HttpClient::get(const std::string& host,
                     const std::string& target,
                     const std::string& cookieHeader,
                     std::string& responseBody,
                     std::string& errorText,
                     std::string* outSetCookie)
{
    return sendWithRedirects(http::verb::get, host, target, cookieHeader,
                             {}, {}, {}, responseBody, errorText, outSetCookie);
}

bool HttpClient::post(const std::string& host,
                      const std::string& target,
                      const std::string& cookieHeader,
                      const std::string& body,
                      const std::string& contentType,
                      std::string& responseBody,
                      std::string& errorText,
                      const std::vector<std::pair<std::string,std::string>>& extraHeaders)
{
    return sendWithRedirects(http::verb::post, host, target, cookieHeader,
                             body, contentType, extraHeaders,
                             responseBody, errorText);
}
