#pragma once

#include <string>
#include <map>
#include <vector>

namespace lunarica {

    class Context {
    public:
        Context();
        ~Context() = default;

        const std::string& getUrl() const;
        void setUrl(const std::string& url);

        void addHeader(const std::string& name, const std::string& value);
        bool removeHeader(const std::string& name);
        const std::map<std::string, std::string>& getHeaders() const;
        void clearHeaders();

        void addQueryParam(const std::string& name, const std::string& value);
        bool removeQueryParam(const std::string& name);
        const std::map<std::string, std::vector<std::string>>& getQueryParams() const;
        void clearQueryParams();

        void addBodyParam(const std::string& name, const std::string& value);
        bool removeBodyParam(const std::string& name);
        const std::map<std::string, std::string>& getBodyParams() const;
        void clearBodyParams();

        bool shouldExit() const;
        void setShouldExit(bool value);

        int getConnectionTimeout() const;
        void setConnectionTimeout(int seconds);
        int getReadTimeout() const;
        void setReadTimeout(int seconds);

    private:
        std::string url_;
        std::map<std::string, std::string> headers_;
        std::map<std::string, std::vector<std::string>> queryParams_;
        std::map<std::string, std::string> bodyParams_;
        bool shouldExit_;
        int connectionTimeout_ = 3;
        int readTimeout_ = 5;
    };

}