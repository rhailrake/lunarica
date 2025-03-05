#pragma once

#include "command.h"
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

namespace lunarica {

    class CommandRegistry {
    public:
        CommandRegistry() = default;
        ~CommandRegistry() = default;

        void registerCommand(std::shared_ptr<Command> command);

        std::shared_ptr<Command> getCommand(const std::string& name) const;

        std::vector<std::shared_ptr<Command>> getAllCommands() const;

        std::vector<std::shared_ptr<Command>> getCommandsByCategory(const std::string& category) const;

        std::vector<std::string> getCategories() const;

        std::vector<std::string> getCommandNames() const;

        bool hasCommand(const std::string& name) const;

        void updateCommandContexts(std::shared_ptr<Context> context);

    private:
        std::map<std::string, std::shared_ptr<Command>> commands_;
        std::unordered_map<std::string, std::vector<std::shared_ptr<Command>>> commandsByCategory_;
    };

}