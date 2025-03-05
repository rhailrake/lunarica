#pragma once

#include <functional>
#include <httplib.h>
#include <iostream>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "json_formatter.h"
#include "core/context.h"

namespace lunarica {

    class HttpService {
    public:
        explicit HttpService(std::shared_ptr<Context> context, std::shared_ptr<JsonFormatter> formatter);
        ~HttpService() = default;

        void get(const std::string& path);
        void post(const std::string& path);
        void put(const std::string& path);
        void del(const std::string& path);

    private:
        std::shared_ptr<Context> context_;
        std::shared_ptr<JsonFormatter> formatter_;

        void makeRequest(const std::string& path,
                        const std::string& method,
                        const std::function<httplib::Result(httplib::Client&, const std::string&, const httplib::Headers&)>& requestFunc);

        void makeRequestWithBody(const std::string& path,
                               const std::string& method,
                               const std::function<httplib::Result(httplib::Client&, const std::string&, const httplib::Headers&, const std::string&, const std::string&)>& requestFunc);

        void processResponse(const httplib::Result& result);

        httplib::Client prepareClient(const std::string& url, std::string& path);

        std::string buildQueryString();

        std::string parseUrl(const std::string& baseUrl, const std::string& path);

        std::string extractHost(const std::string& url);

        std::string buildRequestBody();
    };

}