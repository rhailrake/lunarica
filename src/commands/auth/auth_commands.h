#pragma once

#include <base64.h>
#include <iostream>
#include <sstream>
#include "core/command.h"

namespace lunarica {

class AuthCommand : public Command {
public:
    explicit AuthCommand(std::shared_ptr<Context> context)
        : Command(context) {}

    std::string getName() const override {
        return "auth";
    }

    std::string getCategory() const override {
        return "auth";
    }

    std::string getDescription() const override {
        return "Manage authentication";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "auth",
            "auth basic username password",
            "auth bearer token123",
            "auth apikey X-API-KEY abcdef123456"
        };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Available authentication methods:" << std::endl;
            std::cout << "  basic <username> <password> - Basic authentication" << std::endl;
            std::cout << "  bearer <token> - Bearer token authentication" << std::endl;
            std::cout << "  apikey <name> <value> - API key authentication" << std::endl;
            return true;
        }

        std::istringstream iss(args);
        std::string method;
        iss >> method;

        std::transform(method.begin(), method.end(), method.begin(),
                      [](unsigned char c){ return std::tolower(c); });

        std::string remainingArgs;
        std::getline(iss >> std::ws, remainingArgs);

        if (method == "basic") {
            return executeBasicAuth(remainingArgs);
        } else if (method == "bearer") {
            return executeBearerAuth(remainingArgs);
        } else if (method == "apikey") {
            return executeApiKey(remainingArgs);
        } else {
            std::cout << "Unknown authentication method: " << method << std::endl;
            std::cout << "Use 'auth' without arguments to see available methods" << std::endl;
            return true;
        }
    }

    std::string getHint() const override {
        return "[method] [args]   - Set authentication method";
    }

private:
    bool executeBasicAuth(const std::string& args) {
        if (args.empty()) {
            std::cout << "Usage: auth basic <username> <password>" << std::endl;
            return true;
        }

        std::istringstream iss(args);
        std::string username, password;
        iss >> username >> password;

        if (username.empty() || password.empty()) {
            std::cout << "Usage: auth basic <username> <password>" << std::endl;
            std::cout << "Both username and password are required" << std::endl;
            return true;
        }

        std::string auth_str = username + ":" + password;
        std::string encoded = base64_encode(auth_str);

        context_->addHeader("Authorization", "Basic " + encoded);
        std::cout << "Added Basic authentication header for user: " << username << std::endl;

        return true;
    }

    bool executeBearerAuth(const std::string& args) {
        if (args.empty()) {
            std::cout << "Usage: auth bearer <token>" << std::endl;
            return true;
        }

        std::string token = args;
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);

        context_->addHeader("Authorization", "Bearer " + token);
        std::cout << "Added Bearer token authentication header" << std::endl;

        return true;
    }

    bool executeApiKey(const std::string& args) {
        if (args.empty()) {
            std::cout << "Usage: auth apikey <name> <value>" << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  auth apikey X-API-KEY mySecretApiKey" << std::endl;
            std::cout << "  auth apikey api_key mySecretApiKey" << std::endl;
            return true;
        }

        std::istringstream iss(args);
        std::string name, value;
        iss >> name >> value;

        if (name.empty() || value.empty()) {
            std::cout << "Usage: auth apikey <name> <value>" << std::endl;
            std::cout << "Both name and value are required" << std::endl;
            return true;
        }

        context_->addHeader(name, value);
        std::cout << "Added API key header: " << name << ": " << value << std::endl;

        return true;
    }
};

}