#include "http_service.h"

namespace lunarica {

HttpService::HttpService(std::shared_ptr<Context> context, std::shared_ptr<JsonFormatter> formatter)
    : context_(std::move(context)), formatter_(std::move(formatter)) {
}

void HttpService::get(const std::string& path) {
    makeRequest(path, "GET", [](httplib::Client& client, const std::string& path, const httplib::Headers& headers) {
        return client.Get(path.c_str(), headers);
    });
}

void HttpService::post(const std::string& path) {
    makeRequestWithBody(path, "POST", [](httplib::Client& client, const std::string& path,
                                      const httplib::Headers& headers, const std::string& body, const std::string& contentType) {
        return client.Post(path.c_str(), headers, body, contentType);
    });
}

void HttpService::put(const std::string& path) {
    makeRequestWithBody(path, "PUT", [](httplib::Client& client, const std::string& path,
                                     const httplib::Headers& headers, const std::string& body, const std::string& contentType) {
        return client.Put(path.c_str(), headers, body, contentType);
    });
}

void HttpService::del(const std::string& path) {
    makeRequest(path, "DELETE", [](httplib::Client& client, const std::string& path, const httplib::Headers& headers) {
        return client.Delete(path.c_str(), headers);
    });
}

void HttpService::makeRequest(const std::string& path,
                             const std::string& method,
                             const std::function<httplib::Result(httplib::Client&, const std::string&, const httplib::Headers&)>& requestFunc) {
    std::string url = parseUrl(context_->getUrl(), path);
    std::string host = extractHost(url);
    std::string queryString = buildQueryString();

    if (!queryString.empty() && url.find('?') == std::string::npos) {
        url += "?" + queryString;
    } else if (!queryString.empty()) {
        url += "&" + queryString;
    }

    std::cout << "\nMaking " << method << " request to: " << url << std::endl;

    std::string requestPath;
    httplib::Client client = prepareClient(url, requestPath);

    httplib::Headers headers;
    for (const auto& [name, value] : context_->getHeaders()) {
        headers.emplace(name, value);
    }

    auto res = requestFunc(client, requestPath, headers);
    processResponse(res);
}

void HttpService::makeRequestWithBody(const std::string& path,
                                    const std::string& method,
                                    const std::function<httplib::Result(httplib::Client&, const std::string&, const httplib::Headers&, const std::string&, const std::string&)>& requestFunc) {
    std::string url = parseUrl(context_->getUrl(), path);
    std::string host = extractHost(url);
    std::string queryString = buildQueryString();

    if (!queryString.empty() && url.find('?') == std::string::npos) {
        url += "?" + queryString;
    } else if (!queryString.empty()) {
        url += "&" + queryString;
    }

    std::cout << "\nMaking " << method << " request to: " << url << std::endl;

    std::string requestPath;
    httplib::Client client = prepareClient(url, requestPath);

    httplib::Headers headers;
    for (const auto& [name, value] : context_->getHeaders()) {
        headers.emplace(name, value);
    }

    std::string body = buildRequestBody();
    std::string contentType = "application/json";

    if (headers.find("Content-Type") == headers.end()) {
        headers.emplace("Content-Type", contentType);
    } else {
        auto it = headers.find("Content-Type");
        contentType = it->second;
    }

    auto res = requestFunc(client, requestPath, headers, body, contentType);
    processResponse(res);
}

void HttpService::processResponse(const httplib::Result& res) {
    if (res) {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "STATUS: " << res->status << std::endl;
        std::cout << std::string(50, '=') << std::endl;

        std::cout << "HEADERS:" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        for (const auto& [name, value] : res->headers) {
            std::cout << "  " << name << ": " << value << std::endl;
        }
        std::cout << std::string(50, '=') << std::endl;

        std::cout << "BODY:" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        if (res->body.empty()) {
            std::cout << "(Empty response)" << std::endl;
        } else {
            formatter_->format(res->body);
        }
        std::cout << std::string(50, '=') << std::endl;
    } else {
        auto err = res.error();
        if (err == httplib::Error::Connection) {
            std::cout << "Error: Could not connect to the server." << std::endl;
            std::cout << "Please check your internet connection or try again later." << std::endl;
        } else if (err == httplib::Error::Read) {
            std::cout << "Error: Server took too long to respond or connection was interrupted." << std::endl;
        } else {
            std::cout << "Error making request: " << httplib::to_string(err) << std::endl;
        }
    }
}

httplib::Client HttpService::prepareClient(const std::string& url, std::string& path) {
    std::string host = extractHost(url);

    size_t pathStart = url.find(host) + host.length();
    path = url.substr(pathStart);
    if (path.empty()) path = "/";

    httplib::Client client(host);
    client.set_connection_timeout(context_->getConnectionTimeout());
    client.set_read_timeout(context_->getReadTimeout());
    client.set_follow_location(true);

    return client;
}

std::string HttpService::buildQueryString() {
    std::string result;
    const auto& params = context_->getQueryParams();

    for (const auto& [name, values] : params) {
        for (const auto& value : values) {
            if (!result.empty()) {
                result += "&";
            }
            result += name + "=" + value;
        }
    }

    return result;
}

std::string HttpService::parseUrl(const std::string& baseUrl, const std::string& path) {
    if (path.empty()) {
        return baseUrl;
    }

    if (path.find("http://") == 0 || path.find("https://") == 0) {
        return path;
    }

    if (path[0] == '/') {
        std::string url = baseUrl;

        size_t protocolEnd = url.find("://");
        if (protocolEnd != std::string::npos) {
            size_t pathStart = url.find('/', protocolEnd + 3);
            if (pathStart != std::string::npos) {
                url = url.substr(0, pathStart);
            }
        }

        return url + path;
    }

    std::string url = baseUrl;
    if (url.back() != '/') {
        url += '/';
    }

    return url + path;
}

std::string HttpService::extractHost(const std::string& url) {
    std::string host = url;

    if (host.find("http://") == 0) {
        host = host.substr(7);
    } else if (host.find("https://") == 0) {
        host = host.substr(8);
    }

    size_t pathStart = host.find('/');
    if (pathStart != std::string::npos) {
        host = host.substr(0, pathStart);
    }

    return host;
}

std::string HttpService::buildRequestBody() {
    nlohmann::json jsonBody = nlohmann::json::object();

    for (const auto& [key, value] : context_->getBodyParams()) {
        try {
            if (value == "true") {
                jsonBody[key] = true;
            } else if (value == "false") {
                jsonBody[key] = false;
            } else if (value == "null") {
                jsonBody[key] = nullptr;
            } else if (std::all_of(value.begin(), value.end(), [](char c) {
                return std::isdigit(c) || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E';
            })) {
                if (value.find('.') != std::string::npos) {
                    jsonBody[key] = std::stod(value);
                } else {
                    jsonBody[key] = std::stoll(value);
                }
            } else {
                jsonBody[key] = value;
            }
        } catch (const std::exception&) {
            jsonBody[key] = value;
        }
    }

    return jsonBody.dump();
}

}