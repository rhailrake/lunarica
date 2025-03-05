#pragma once

#include <iostream>
#include "core/command.h"

namespace lunarica {

    class CdCommand : public Command {
    public:
        explicit CdCommand(std::shared_ptr<Context> context) : Command(context) {}

        std::string getName() const override {
            return "cd";
        }

        std::string getCategory() const override {
            return "system";
        }

        std::string getDescription() const override {
            return "Change the current URL";
        }

        std::vector<std::string> getExamples() const override {
            return {
                "cd https://api.example.com"
            };
        }

        bool execute(const std::string& args) override {
            if (args.empty()) {
                std::cout << "Current URL: " << context_->getUrl() << std::endl;
                return true;
            }

            context_->setUrl(args);
            std::cout << "Current URL set to: " << context_->getUrl() << std::endl;
            return true;
        }

        std::string getHint() const override {
            return "<url>          - Change the current URL";
        }
    };

}