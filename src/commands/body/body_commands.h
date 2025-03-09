#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <json/json.h>
#include "core/command.h"
#include "utils/encoding_utils.h"

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
            Encoding encoding = EncodingUtils::detectFileEncoding(filename);
            std::string content;

            if (!EncodingUtils::readFileToUTF8(filename, content)) {
                std::cout << "Error: Could not open or read file " << filename << std::endl;
                return true;
            }

            if (content.empty()) {
                std::cout << "Error: File is empty" << std::endl;
                return true;
            }

            if (encoding != Encoding::UTF8 && encoding != Encoding::Unknown) {
                std::cout << "Note: File was automatically converted from "
                          << EncodingUtils::getEncodingName(encoding)
                          << " to UTF-8" << std::endl;
            }

            Json::CharReaderBuilder builder;
            builder["allowComments"] = true;
            builder["allowTrailingCommas"] = true;
            builder["strictRoot"] = false;
            builder["collectComments"] = false;

            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            Json::Value jsonData;
            std::string parseErrors;

            bool parseSuccess = reader->parse(
                content.c_str(), content.c_str() + content.size(),
                &jsonData, &parseErrors
            );

            try {
                if (!parseSuccess) {
                    std::cout << "Error parsing JSON data: " << parseErrors << std::endl;
                    std::cout << "First 50 characters of file: "
                              << content.substr(0, std::min(size_t(50), content.size())) << std::endl;

                    std::cout << "First 20 bytes (hex): ";
                    for (size_t i = 0; i < std::min(size_t(20), content.size()); ++i) {
                        std::cout << std::hex << std::setw(2) << std::setfill('0')
                                << static_cast<int>(static_cast<unsigned char>(content[i])) << " ";
                    }
                    std::cout << std::dec << std::endl;
                    return true;
                }

                if (!jsonData.isObject()) {
                    std::cout << "Error: JSON file must contain an object" << std::endl;
                    return true;
                }

                context_->clearBodyParams();

                int count = 0;
                Json::Value::Members members = jsonData.getMemberNames();
                for (const auto& key : members) {
                    std::string value;
                    if (jsonData[key].isString()) {
                        value = jsonData[key].asString();
                    } else {
                        Json::FastWriter writer;
                        value = writer.write(jsonData[key]);
                        if (!value.empty() && value[value.length()-1] == '\n') {
                            value.erase(value.length()-1);
                        }
                    }
                    context_->addBodyParam(key, value);
                    count++;
                }

                std::cout << "Loaded " << count << " body parameters from " << filename << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error processing JSON data: " << e.what() << std::endl;
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