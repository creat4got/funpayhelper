#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include <vector>
#include <utility>

class HttpClient
{
public:
    HttpClient();

    // outSetCookie: если не nullptr — сюда записываются все Set-Cookie заголовки ответа
    bool get(const std::string& host,
             const std::string& target,
             const std::string& cookieHeader,
             std::string& responseBody,
             std::string& errorText,
             std::string* outSetCookie = nullptr);

    // extraHeaders: дополнительные заголовки, напр. {{"X-Requested-With","XMLHttpRequest"}}
    bool post(const std::string& host,
              const std::string& target,
              const std::string& cookieHeader,
              const std::string& body,
              const std::string& contentType,
              std::string& responseBody,
              std::string& errorText,
              const std::vector<std::pair<std::string,std::string>>& extraHeaders = {});
};

#endif // HTTPCLIENT_H
