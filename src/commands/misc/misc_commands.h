#pragma once

#include <iostream>
#include <sstream>
#include "core/command.h"

namespace lunarica {

class ClearCommand : public Command {
public:
    explicit ClearCommand(std::shared_ptr<Context> context)
        : Command(context) {}

    std::string getName() const override {
        return "clear-params";
    }

    std::string getCategory() const override {
        return "misc";
    }

    std::string getDescription() const override {
        return "Clear all parameters and headers";
    }

    std::vector<std::string> getExamples() const override {
        return { "clear" };
    }

    bool execute(const std::string& args) override {
        context_->clearHeaders();
        context_->clearQueryParams();
        context_->clearBodyParams();
        std::cout << "Cleared all headers, query parameters, and body parameters" << std::endl;
        return true;
    }

    std::string getHint() const override {
        return "                   - Clear all parameters and headers";
    }
};

class TimeoutCommand : public Command {
public:
    explicit TimeoutCommand(std::shared_ptr<Context> context)
        : Command(context) {}

    std::string getName() const override {
        return "timeout";
    }

    std::string getCategory() const override {
        return "misc";
    }

    std::string getDescription() const override {
        return "Set connection and read timeouts";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "timeout",
            "timeout 5 10"
        };
    }

    bool execute(const std::string& args) override {
        std::istringstream iss(args);
        int connTimeout, readTimeout;

        if (args.empty()) {
            std::cout << "Current timeouts:" << std::endl;
            std::cout << "  Connection timeout: " << context_->getConnectionTimeout() << " seconds" << std::endl;
            std::cout << "  Read timeout: " << context_->getReadTimeout() << " seconds" << std::endl;
            return true;
        }

        if (!(iss >> connTimeout >> readTimeout)) {
            std::cout << "Usage: timeout <connection_timeout> <read_timeout>" << std::endl;
            std::cout << "  Both values are in seconds" << std::endl;
            return true;
        }

        if (connTimeout <= 0 || readTimeout <= 0) {
            std::cout << "Error: Timeout values must be positive" << std::endl;
            return true;
        }

        context_->setConnectionTimeout(connTimeout);
        context_->setReadTimeout(readTimeout);

        std::cout << "Timeouts set:" << std::endl;
        std::cout << "  Connection timeout: " << context_->getConnectionTimeout() << " seconds" << std::endl;
        std::cout << "  Read timeout: " << context_->getReadTimeout() << " seconds" << std::endl;

        return true;
    }

    std::string getHint() const override {
        return "<conn> <read>   - Set connection and read timeouts in seconds";
    }
};

class ParamsCommand : public Command {
public:
    explicit ParamsCommand(std::shared_ptr<Context> context)
        : Command(context) {}

    std::string getName() const override {
        return "params";
    }

    std::string getCategory() const override {
        return "misc";
    }

    std::string getDescription() const override {
        return "Show all parameters";
    }

    std::vector<std::string> getExamples() const override {
        return { "params" };
    }

    bool execute(const std::string& args) override {
        const auto& headers = context_->getHeaders();
        if (headers.empty()) {
            std::cout << "No headers set" << std::endl;
        } else {
            std::cout << "Current headers:" << std::endl;
            for (const auto& [name, value] : headers) {
                std::cout << "  " << name << ": " << value << std::endl;
            }
        }

        std::cout << std::endl;

        const auto& bodyParams = context_->getBodyParams();
        if (bodyParams.empty()) {
            std::cout << "No body parameters set" << std::endl;
        } else {
            std::cout << "Current body parameters:" << std::endl;
            for (const auto& [name, value] : bodyParams) {
                std::cout << "  " << name << " = " << value << std::endl;
            }
        }

        std::cout << std::endl;

        const auto& queryParams = context_->getQueryParams();
        if (queryParams.empty()) {
            std::cout << "No query parameters set" << std::endl;
        } else {
            std::cout << "Current query parameters:" << std::endl;
            for (const auto& [name, values] : queryParams) {
                for (const auto& value : values) {
                    std::cout << "  " << name << " = " << value << std::endl;
                }
            }
        }

        return true;
    }

    std::string getHint() const override {
        return "                   - Show all parameters";
    }
};

class ClearScreenCommand : public Command {
public:
    explicit ClearScreenCommand(std::shared_ptr<Context> context)
        : Command(context) {}

    std::string getName() const override {
        return "clear";
    }

    std::vector<std::string> getAliases() const override {
        return { "cls" };
    }

    std::string getCategory() const override {
        return "misc";
    }

    std::string getDescription() const override {
        return "Clear the console screen";
    }

    std::vector<std::string> getExamples() const override {
        return { "clear-screen", "cls" };
    }

    bool execute(const std::string& args) override {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        std::cout << "Console cleared" << std::endl;
        return true;
    }

    std::string getHint() const override {
        return "                - Clear the console screen";
    }
};

}