#pragma once

#include <iostream>
#include "core/command.h"
#include "services/http_service.h"

namespace lunarica {

class HttpCommand : public Command {
public:
    explicit HttpCommand(std::shared_ptr<Context> context,
                         std::shared_ptr<HttpService> httpService)
        : Command(context), httpService_(httpService) {}

    std::string getCategory() const override {
        return "network";
    }

protected:
    std::shared_ptr<HttpService> httpService_;
};

class GetCommand : public HttpCommand {
public:
    explicit GetCommand(std::shared_ptr<Context> context,
                        std::shared_ptr<HttpService> httpService)
        : HttpCommand(context, httpService) {}

    std::string getName() const override {
        return "get";
    }

    std::string getDescription() const override {
        return "Make a GET request to the specified path";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "get /users",
            "get /api/products?id=123"
        };
    }

    bool execute(const std::string& args) override {
        httpService_->get(args);
        return true;
    }

    std::string getHint() const override {
        return "<path>         - Make a GET request";
    }
};

class PostCommand : public HttpCommand {
public:
    explicit PostCommand(std::shared_ptr<Context> context,
                         std::shared_ptr<HttpService> httpService)
        : HttpCommand(context, httpService) {}

    std::string getName() const override {
        return "post";
    }

    std::string getDescription() const override {
        return "Make a POST request to the specified path";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "post /users",
            "post /api/login"
        };
    }

    bool execute(const std::string& args) override {
        httpService_->post(args);
        return true;
    }

    std::string getHint() const override {
        return "<path>         - Make a POST request";
    }
};

class PutCommand : public HttpCommand {
public:
    explicit PutCommand(std::shared_ptr<Context> context,
                        std::shared_ptr<HttpService> httpService)
        : HttpCommand(context, httpService) {}

    std::string getName() const override {
        return "put";
    }

    std::string getDescription() const override {
        return "Make a PUT request to the specified path";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "put /users/1",
            "put /api/products/123"
        };
    }

    bool execute(const std::string& args) override {
        httpService_->put(args);
        return true;
    }

    std::string getHint() const override {
        return "<path>         - Make a PUT request";
    }
};

class DeleteCommand : public HttpCommand {
public:
    explicit DeleteCommand(std::shared_ptr<Context> context,
                           std::shared_ptr<HttpService> httpService)
        : HttpCommand(context, httpService) {}

    std::string getName() const override {
        return "delete";
    }

    std::string getDescription() const override {
        return "Make a DELETE request to the specified path";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "delete /users/1",
            "delete /api/products/123"
        };
    }

    bool execute(const std::string& args) override {
        httpService_->del(args);
        return true;
    }

    std::string getHint() const override {
        return "<path>         - Make a DELETE request";
    }
};

}