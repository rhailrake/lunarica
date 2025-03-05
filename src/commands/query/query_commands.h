#pragma once

#include <iostream>
#include "core/command.h"

namespace lunarica {

class QueryBaseCommand : public Command {
public:
    explicit QueryBaseCommand(std::shared_ptr<Context> context)
        : Command(context) {}

    std::string getCategory() const override {
        return "query";
    }
};

class QueryCommand : public QueryBaseCommand {
public:
    explicit QueryCommand(std::shared_ptr<Context> context)
        : QueryBaseCommand(context) {}

    std::string getName() const override {
        return "query";
    }

    std::string getDescription() const override {
        return "Add a query parameter";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "query page=1",
            "query sort=asc"
        };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Usage: query <name>=<value>" << std::endl;
            std::cout << "Example: query page=1" << std::endl;
            return true;
        }

        size_t equalsPos = args.find('=');
        if (equalsPos == std::string::npos) {
            std::cout << "Invalid query parameter format. Use 'query <name>=<value>'" << std::endl;
            std::cout << "Example: query page=1" << std::endl;
            return true;
        }

        std::string name = args.substr(0, equalsPos);
        std::string value = args.substr(equalsPos + 1);

        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        context_->addQueryParam(name, value);
        std::cout << "Added query parameter: " << name << "=" << value << std::endl;

        return true;
    }

    std::string getHint() const override {
        return "<name>=<value> - Add a query parameter";
    }
};

class QueryParamsCommand : public QueryBaseCommand {
public:
    explicit QueryParamsCommand(std::shared_ptr<Context> context)
        : QueryBaseCommand(context) {}

    std::string getName() const override {
        return "query-params";
    }

    std::string getDescription() const override {
        return "Show all query parameters";
    }

    std::vector<std::string> getExamples() const override {
        return { "query-params" };
    }

    bool execute(const std::string& args) override {
        const auto& params = context_->getQueryParams();

        if (params.empty()) {
            std::cout << "No query parameters set" << std::endl;
            return true;
        }

        std::cout << "Current query parameters:" << std::endl;
        for (const auto& [name, values] : params) {
            for (const auto& value : values) {
                std::cout << "  " << name << " = " << value << std::endl;
            }
        }

        return true;
    }

    std::string getHint() const override {
        return "                - Show all query parameters";
    }
};

class RemoveQueryParamCommand : public QueryBaseCommand {
public:
    explicit RemoveQueryParamCommand(std::shared_ptr<Context> context)
        : QueryBaseCommand(context) {}

    std::string getName() const override {
        return "rm-query";
    }

    std::string getDescription() const override {
        return "Remove a query parameter";
    }

    std::vector<std::string> getExamples() const override {
        return { "rm-query page" };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Usage: rm-query <name>" << std::endl;
            return true;
        }

        std::string name = args;
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);

        context_->removeQueryParam(name);
        std::cout << "Removed query parameter: " << name << std::endl;

        return true;
    }

    std::string getHint() const override {
        return "<name>     - Remove a query parameter";
    }
};

}