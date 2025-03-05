#include "command_registry.h"

namespace lunarica {

void CommandRegistry::registerCommand(std::shared_ptr<Command> command) {
    if (command) {
        const std::string& name = command->getName();
        const std::string& category = command->getCategory();

        commands_[name] = command;

        for (const auto& alias : command->getAliases()) {
            commands_[alias] = command;
        }

        commandsByCategory_[category].push_back(command);
    }
}

std::shared_ptr<Command> CommandRegistry::getCommand(const std::string& name) const {
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Command>> CommandRegistry::getAllCommands() const {
    std::vector<std::shared_ptr<Command>> result;
    result.reserve(commands_.size());

    for (const auto& [name, cmd] : commands_) {
        result.push_back(cmd);
    }

    return result;
}

std::vector<std::shared_ptr<Command>> CommandRegistry::getCommandsByCategory(const std::string& category) const {
    auto it = commandsByCategory_.find(category);
    if (it != commandsByCategory_.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::string> CommandRegistry::getCategories() const {
    std::vector<std::string> result;
    result.reserve(commandsByCategory_.size());

    for (const auto& [category, _] : commandsByCategory_) {
        result.push_back(category);
    }

    return result;
}

std::vector<std::string> CommandRegistry::getCommandNames() const {
    std::vector<std::string> result;
    result.reserve(commands_.size());

    for (const auto& [name, _] : commands_) {
        result.push_back(name);
    }

    return result;
}

bool CommandRegistry::hasCommand(const std::string& name) const {
    return commands_.find(name) != commands_.end();
}

void CommandRegistry::updateCommandContexts(std::shared_ptr<Context> context) {
    for (auto& [_, cmd] : commands_) {
        cmd->setContext(context);
    }
}

}