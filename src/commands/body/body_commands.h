#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "core/command.h"

namespace lunarica {

class BodyBaseCommand : public Command {
public:
    explicit BodyBaseCommand(std::shared_ptr<Context> context)
        : Command(context) {}

    std::string getCategory() const override {
        return "body";
    }
};

class BodyCommand : public BodyBaseCommand {
public:
    explicit BodyCommand(std::shared_ptr<Context> context)
        : BodyBaseCommand(context) {}

    std::string getName() const override {
        return "body";
    }

    std::string getDescription() const override {
        return "Add or update a body parameter";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "body username=john",
            "body active=true",
            "body score=42"
        };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Usage: body <name>=<value>" << std::endl;
            std::cout << "Example: body username=john" << std::endl;
            return true;
        }

        size_t equalsPos = args.find('=');
        if (equalsPos == std::string::npos) {
            std::cout << "Invalid body parameter format. Use 'body <name>=<value>'" << std::endl;
            std::cout << "Example: body username=john" << std::endl;
            return true;
        }

        std::string name = args.substr(0, equalsPos);
        std::string value = args.substr(equalsPos + 1);

        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        bool isSpecialValue = false;
        if (value == "true" || value == "false" || value == "null") {
            isSpecialValue = true;
        } else if (std::all_of(value.begin(), value.end(), [](char c) {
            return std::isdigit(c) || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E';
        })) {
            isSpecialValue = true;
            std::cout << "Added body parameter: " << name << " = " << value << " (number)" << std::endl;
            context_->addBodyParam(name, value);
            return true;
        }

        context_->addBodyParam(name, value);
        if (isSpecialValue) {
            std::cout << "Added body parameter: " << name << " = " << value << " (special value)" << std::endl;
        } else {
            std::cout << "Added body parameter: " << name << " = " << value << std::endl;
        }

        return true;
    }

    std::string getHint() const override {
        return "<name>=<value> - Add or update a body parameter";
    }
};

class BodyParamsCommand : public BodyBaseCommand {
public:
    explicit BodyParamsCommand(std::shared_ptr<Context> context)
        : BodyBaseCommand(context) {}

    std::string getName() const override {
        return "body-params";
    }

    std::string getDescription() const override {
        return "Show all body parameters";
    }

    std::vector<std::string> getExamples() const override {
        return { "body-params" };
    }

    bool execute(const std::string& args) override {
        const auto& params = context_->getBodyParams();

        if (params.empty()) {
            std::cout << "No body parameters set" << std::endl;
            return true;
        }

        std::cout << "Current body parameters:" << std::endl;
        for (const auto& [name, value] : params) {
            std::cout << "  " << name << " = " << value << std::endl;
        }

        return true;
    }

    std::string getHint() const override {
        return "                - Show all body parameters";
    }
};

class RemoveBodyParamCommand : public BodyBaseCommand {
public:
    explicit RemoveBodyParamCommand(std::shared_ptr<Context> context)
        : BodyBaseCommand(context) {}

    std::string getName() const override {
        return "rm-body";
    }

    std::string getDescription() const override {
        return "Remove a body parameter";
    }

    std::vector<std::string> getExamples() const override {
        return { "rm-body username" };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Usage: rm-body <name>" << std::endl;
            return true;
        }

        std::string name = args;
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);

        context_->removeBodyParam(name);
        std::cout << "Removed body parameter: " << name << std::endl;

        return true;
    }

    std::string getHint() const override {
        return "<name>     - Remove a body parameter";
    }
};

class ClearBodyCommand : public BodyBaseCommand {
public:
    explicit ClearBodyCommand(std::shared_ptr<Context> context)
        : BodyBaseCommand(context) {}

    std::string getName() const override {
        return "clear-body";
    }

    std::string getDescription() const override {
        return "Clear all body parameters";
    }

    std::vector<std::string> getExamples() const override {
        return { "clear-body" };
    }

    bool execute(const std::string& args) override {
        context_->clearBodyParams();
        std::cout << "Cleared all body parameters" << std::endl;
        return true;
    }

    std::string getHint() const override {
        return "                - Clear all body parameters";
    }
};

class LoadBodyCommand : public BodyBaseCommand {
public:
    explicit LoadBodyCommand(std::shared_ptr<Context> context)
        : BodyBaseCommand(context) {}

    std::string getName() const override {
        return "load-body";
    }

    std::string getDescription() const override {
        return "Load body parameters from a JSON file";
    }

    std::vector<std::string> getExamples() const override {
        return { "load-body request.json" };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Usage: load-body <filename>" << std::endl;
            return true;
        }

        std::string filename = args;
        filename.erase(0, filename.find_first_not_of(" \t"));
        filename.erase(filename.find_last_not_of(" \t") + 1);

        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cout << "Error: Could not open file " << filename << std::endl;
                return true;
            }

            file.seekg(0, std::ios::end);
            const std::streampos fileSize = file.tellg();
            if (fileSize == 0) {
                std::cout << "Error: File is empty" << std::endl;
                return true;
            }
            file.seekg(0, std::ios::beg);

            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());

            content.erase(0, content.find_first_not_of(" \t\n\r\f\v"));
            content.erase(content.find_last_not_of(" \t\n\r\f\v") + 1);

            if (content.empty()) {
                std::cout << "Error: File contains only whitespace" << std::endl;
                return true;
            }

            if (content.size() >= 3 &&
                static_cast<unsigned char>(content[0]) == 0xEF &&
                static_cast<unsigned char>(content[1]) == 0xBB &&
                static_cast<unsigned char>(content[2]) == 0xBF) {
                content = content.substr(3);
            }

            if (content.front() != '{' && content.front() != '[') {
                std::cout << "Error: File does not appear to be valid JSON. "
                          << "JSON should start with '{' or '['" << std::endl;
                return true;
            }

            nlohmann::json jsonData;
            try {
                jsonData = nlohmann::json::parse(content);

                if (!jsonData.is_object()) {
                    std::cout << "Error: JSON file must contain an object" << std::endl;
                    return true;
                }

                context_->clearBodyParams();

                int count = 0;
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    std::string value;
                    if (it.value().is_string()) {
                        value = it.value().get<std::string>();
                    } else {
                        value = it.value().dump();
                    }
                    context_->addBodyParam(it.key(), value);
                    count++;
                }

                std::cout << "Loaded " << count << " body parameters from " << filename << std::endl;
            } catch (const nlohmann::json::exception& e) {
                std::cout << "Error parsing JSON data: " << e.what() << std::endl;
                std::cout << "First 50 characters of file: "
                          << content.substr(0, std::min(size_t(50), content.size())) << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error loading file: " << e.what() << std::endl;
        }

        return true;
    }

    std::string getHint() const override {
        return "<filename>   - Load body parameters from JSON file";
    }
};

}