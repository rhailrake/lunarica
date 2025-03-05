#pragma once

#include <iostream>
#include "core/command.h"

namespace lunarica {

    class ExitCommand : public Command {
    public:
        explicit ExitCommand(std::shared_ptr<Context> context) : Command(context) {}

        std::string getName() const override {
            return "exit";
        }

        std::vector<std::string> getAliases() const override {
            return { "quit", "q" };
        }

        std::string getCategory() const override {
            return "system";
        }

        std::string getDescription() const override {
            return "Exit the application";
        }

        std::vector<std::string> getExamples() const override {
            return { "exit", "quit" };
        }

        bool execute(const std::string& args) override {
            std::cout << "Goodbye!" << std::endl;
            context_->setShouldExit(true);
            return true;
        }

        std::string getHint() const override {
            return "                   - Exit the application";
        }
    };

}