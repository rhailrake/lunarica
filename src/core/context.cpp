#include "context.h"

namespace lunarica {

    Context::Context()
        : url_("http://localhost:8000"),
          shouldExit_(false) {
    }

    const std::string& Context::getUrl() const {
        return url_;
    }

    void Context::setUrl(const std::string& url) {
        url_ = url;
    }

    void Context::addHeader(const std::string& name, const std::string& value) {
        headers_[name] = value;
    }

    bool Context::removeHeader(const std::string& name) {
        return headers_.erase(name) > 0;
    }

    const std::map<std::string, std::string>& Context::getHeaders() const {
        return headers_;
    }

    void Context::clearHeaders() {
        headers_.clear();
    }

    void Context::addQueryParam(const std::string& name, const std::string& value) {
        queryParams_[name].push_back(value);
    }

    bool Context::removeQueryParam(const std::string& name) {
        return queryParams_.erase(name) > 0;
    }

    const std::map<std::string, std::vector<std::string>>& Context::getQueryParams() const {
        return queryParams_;
    }

    void Context::clearQueryParams() {
        queryParams_.clear();
    }

    void Context::addBodyParam(const std::string& name, const std::string& value) {
        bodyParams_[name] = value;
    }

    bool Context::removeBodyParam(const std::string& name) {
        return bodyParams_.erase(name) > 0;
    }

    const std::map<std::string, std::string>& Context::getBodyParams() const {
        return bodyParams_;
    }

    void Context::clearBodyParams() {
        bodyParams_.clear();
    }

    bool Context::shouldExit() const {
        return shouldExit_;
    }

    void Context::setShouldExit(bool value) {
        shouldExit_ = value;
    }

    int Context::getConnectionTimeout() const {
        return connectionTimeout_;
    }

    void Context::setConnectionTimeout(int seconds) {
        connectionTimeout_ = seconds;
    }

    int Context::getReadTimeout() const {
        return readTimeout_;
    }

    void Context::setReadTimeout(int seconds) {
        readTimeout_ = seconds;
    }

}