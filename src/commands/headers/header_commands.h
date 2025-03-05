#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include "core/command.h"

namespace lunarica {

class HeaderBaseCommand : public Command {
public:
    explicit HeaderBaseCommand(std::shared_ptr<Context> context)
        : Command(context) {}

    std::string getCategory() const override {
        return "headers";
    }
};

class HeadersCommand : public HeaderBaseCommand {
public:
    explicit HeadersCommand(std::shared_ptr<Context> context)
        : HeaderBaseCommand(context) {}

    std::string getName() const override {
        return "headers";
    }

    std::string getDescription() const override {
        return "Show all headers";
    }

    std::vector<std::string> getExamples() const override {
        return { "headers" };
    }

    bool execute(const std::string& args) override {
        const auto& headers = context_->getHeaders();

        if (headers.empty()) {
            std::cout << "No headers set" << std::endl;
            return true;
        }

        std::cout << "Current headers:" << std::endl;
        for (const auto& [name, value] : headers) {
            std::cout << "  " << name << ": " << value << std::endl;
        }

        return true;
    }

    std::string getHint() const override {
        return "                  - List all headers";
    }
};

class HeaderCommand : public HeaderBaseCommand {
public:
    explicit HeaderCommand(std::shared_ptr<Context> context)
        : HeaderBaseCommand(context) {}

    std::string getName() const override {
        return "header";
    }

    std::string getDescription() const override {
        return "Add or update a header";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "header Content-Type:application/json",
            "header Authorization:Bearer token123"
        };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Usage: header <name>:<value>" << std::endl;
            std::cout << "Example: header Content-Type:application/json" << std::endl;
            return true;
        }

        size_t colonPos = args.find(':');
        if (colonPos == std::string::npos) {
            std::cout << "Invalid header format. Use 'header <name>:<value>'" << std::endl;
            std::cout << "Example: header Content-Type:application/json" << std::endl;
            return true;
        }

        std::string name = args.substr(0, colonPos);
        std::string value = args.substr(colonPos + 1);

        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        context_->addHeader(name, value);
        std::cout << "Added header: " << name << ": " << value << std::endl;

        return true;
    }

    std::string getHint() const override {
        return "<name>:<value>  - Add or update a header";
    }
};

class RemoveHeaderCommand : public HeaderBaseCommand {
public:
    explicit RemoveHeaderCommand(std::shared_ptr<Context> context)
        : HeaderBaseCommand(context) {}

    std::string getName() const override {
        return "rm-header";
    }

    std::string getDescription() const override {
        return "Remove a header";
    }

    std::vector<std::string> getExamples() const override {
        return { "rm-header Content-Type" };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Usage: rm-header <name>" << std::endl;
            return true;
        }

        std::string name = args;
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);

        context_->removeHeader(name);
        std::cout << "Removed header: " << name << std::endl;

        return true;
    }

    std::string getHint() const override {
        return "<name>     - Remove a header";
    }
};

class LoadHeadersCommand : public HeaderBaseCommand {
public:
    explicit LoadHeadersCommand(std::shared_ptr<Context> context)
        : HeaderBaseCommand(context) {}

    std::string getName() const override {
        return "load-headers";
    }

    std::vector<std::string> getAliases() const override {
        return { "load-header" };
    }

    std::string getDescription() const override {
        return "Load headers from a file";
    }

    std::vector<std::string> getExamples() const override {
        return { "load-headers headers.txt" };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            std::cout << "Usage: load-headers <filename>" << std::endl;
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

            std::string line;
            int count = 0;

            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') {
                    continue;
                }

                size_t equalsPos = line.find('=');
                if (equalsPos == std::string::npos) {
                    std::cout << "Warning: Skipping invalid line: " << line << std::endl;
                    continue;
                }

                std::string name = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);

                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                context_->addHeader(name, value);
                count++;
            }

            std::cout << "Loaded " << count << " headers from " << filename << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error loading headers file: " << e.what() << std::endl;
        }

        return true;
    }

    std::string getHint() const override {
        return "<filename>  - Load headers from file (Key=Value format)";
    }
};

}