#pragma once

#include <iomanip>
#include <iostream>
#include <map>
#include "core/command.h"
#include "core/command_registry.h"

namespace lunarica {

class HelpCommand : public Command {
public:
    explicit HelpCommand(std::shared_ptr<Context> context, const CommandRegistry& registry)
        : Command(context), registry_(registry) {}

    std::string getName() const override {
        return "help";
    }

    std::string getCategory() const override {
        return "system";
    }

    std::string getDescription() const override {
        return "Show help information for available commands";
    }

    std::vector<std::string> getExamples() const override {
        return {
            "help",
            "help get"
        };
    }

    bool execute(const std::string& args) override {
        if (args.empty()) {
            showAllCommands();
        } else {
            showCommandHelp(args);
        }
        return true;
    }

    std::string getHint() const override {
        return "[command]            - Show help information";
    }

private:
    const CommandRegistry& registry_;

    void showAllCommands() {
        std::cout << "Available commands:" << std::endl << std::endl;

        auto categories = registry_.getCategories();
        std::sort(categories.begin(), categories.end());

        for (const auto& category : categories) {
            std::string title = category;
            std::transform(title.begin(), title.end(), title.begin(), ::toupper);

            std::cout << "--- " << title << " ---" << std::endl;

            auto commands = registry_.getCommandsByCategory(category);

            std::sort(commands.begin(), commands.end(),
                [](const auto& a, const auto& b) { return a->getName() < b->getName(); });

            for (const auto& cmd : commands) {
                std::cout << "  " << std::left << std::setw(25) << cmd->getName()
                          << "- " << cmd->getDescription() << std::endl;
            }

            std::cout << std::endl;
        }

        std::cout << "For more detailed help on a specific command, type 'help <command>'" << std::endl;
    }

    void showCommandHelp(const std::string& commandName) {
        auto command = registry_.getCommand(commandName);

        if (!command) {
            std::cout << "Unknown command: " << commandName << std::endl;
            return;
        }

        std::cout << "Command: " << command->getName() << std::endl;
        std::cout << "Category: " << command->getCategory() << std::endl;
        std::cout << "Description: " << command->getDescription() << std::endl;

        std::cout << std::endl << "Usage: " << command->getName() << " " << command->getHint() << std::endl;

        auto examples = command->getExamples();
        if (!examples.empty()) {
            std::cout << std::endl << "Examples:" << std::endl;
            for (const auto& example : examples) {
                std::cout << "  " << example << std::endl;
            }
        }
    }
};

}