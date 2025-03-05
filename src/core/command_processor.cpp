#include "command_processor.h"

namespace lunarica {

CommandProcessor::CommandProcessor(std::shared_ptr<Context> context)
    : context_(std::move(context)) {
    jsonFormatter_ = std::make_shared<JsonFormatter>();
    httpService_ = std::make_shared<HttpService>(context_, jsonFormatter_);

    registerCommands();
}

void CommandProcessor::registerCommands() {
    // System commands
    commandRegistry_.registerCommand(std::make_shared<ExitCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<HelpCommand>(context_, commandRegistry_));
    commandRegistry_.registerCommand(std::make_shared<CdCommand>(context_));

    // Network commands
    commandRegistry_.registerCommand(std::make_shared<GetCommand>(context_, httpService_));
    commandRegistry_.registerCommand(std::make_shared<PostCommand>(context_, httpService_));
    commandRegistry_.registerCommand(std::make_shared<PutCommand>(context_, httpService_));
    commandRegistry_.registerCommand(std::make_shared<DeleteCommand>(context_, httpService_));

    // Header commands
    commandRegistry_.registerCommand(std::make_shared<HeadersCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<HeaderCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<RemoveHeaderCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<LoadHeadersCommand>(context_));

    // Body commands
    commandRegistry_.registerCommand(std::make_shared<BodyCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<BodyParamsCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<RemoveBodyParamCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<ClearBodyCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<LoadBodyCommand>(context_));

    // Query commands
    commandRegistry_.registerCommand(std::make_shared<QueryCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<QueryParamsCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<RemoveQueryParamCommand>(context_));

    // Auth commands
    commandRegistry_.registerCommand(std::make_shared<AuthCommand>(context_));

    // Misc commands
    commandRegistry_.registerCommand(std::make_shared<ClearCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<TimeoutCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<ParamsCommand>(context_));
    commandRegistry_.registerCommand(std::make_shared<ClearScreenCommand>(context_));
}

bool CommandProcessor::process(const std::string& commandLine) {
    if (commandLine.empty()) {
        return true;
    }

    auto [cmdName, args] = parseCommandLine(commandLine);

    auto command = commandRegistry_.getCommand(cmdName);
    if (!command) {
        std::cout << "Unknown command: " << cmdName << std::endl;
        std::cout << "Type 'help' to see available commands" << std::endl;
        return true;
    }

    std::cout << "---------------------------------------------------" << std::endl;
    bool result = command->execute(args);
    std::cout << "---------------------------------------------------" << std::endl;

    return result;
}

std::pair<std::string, std::string> CommandProcessor::parseCommandLine(const std::string& commandLine) {
    std::string cmdName;
    std::string args;

    std::istringstream iss(commandLine);
    iss >> cmdName;

    std::getline(iss >> std::ws, args);

    std::transform(cmdName.begin(), cmdName.end(), cmdName.begin(),
                  [](unsigned char c){ return std::tolower(c); });

    return {cmdName, args};
}

std::vector<replxx::Replxx::Completion> CommandProcessor::getCompletions(
    const std::string& input, int& context_len) {

    std::vector<replxx::Replxx::Completion> completions;

    if (input.empty()) {
        for (const auto& cmdName : commandRegistry_.getCommandNames()) {
            completions.emplace_back(cmdName);
        }
        context_len = 0;
        return completions;
    }

    auto commandNames = commandRegistry_.getCommandNames();
    for (const auto& cmd : commandNames) {
        if (cmd.find(input) == 0) {
            completions.emplace_back(cmd);
        }
    }

    size_t spacePos = input.find(' ');
    if (spacePos != std::string::npos) {
        std::string cmdName = input.substr(0, spacePos);
        std::string argPrefix = input.substr(spacePos + 1);

        auto command = commandRegistry_.getCommand(cmdName);
        if (command) {
            auto cmdCompletions = command->getCompletions(argPrefix);
            for (const auto& comp : cmdCompletions) {
                completions.emplace_back(cmdName + " " + comp);
            }
        }
    }

    context_len = input.length();
    return completions;
}

void CommandProcessor::highlightSyntax(const std::string& input,
                                     replxx::Replxx::colors_t& colors) {
    colors.resize(input.size());
    std::fill(colors.begin(), colors.end(), replxx::Replxx::Color::DEFAULT);

    auto commandNames = commandRegistry_.getCommandNames();

    size_t firstSpace = input.find(' ');
    std::string firstWord = input.substr(0, firstSpace);

    if (std::find(commandNames.begin(), commandNames.end(), firstWord) != commandNames.end()) {
        for (size_t i = 0; i < firstWord.size() && i < colors.size(); ++i) {
            colors[i] = replxx::Replxx::Color::BRIGHTMAGENTA;
        }
    }

    if (firstSpace != std::string::npos && firstSpace + 1 < input.size()) {
        size_t colonPos = input.find(':', firstSpace);
        if (colonPos != std::string::npos) {
            for (size_t i = firstSpace + 1; i < colonPos && i < colors.size(); ++i) {
                colors[i] = replxx::Replxx::Color::BRIGHTBLUE;
            }
            for (size_t i = colonPos + 1; i < input.size() && i < colors.size(); ++i) {
                colors[i] = replxx::Replxx::Color::BRIGHTGREEN;
            }
        }
    }
}

std::vector<std::string> CommandProcessor::getHint(const std::string& input,
                                                int& context_len,
                                                replxx::Replxx::Color& color) {
    std::vector<std::string> hints;

    if (input.empty()) {
        return hints;
    }

    color = replxx::Replxx::Color::BRIGHTBLUE;
    context_len = input.length();

    std::string cmdName = input;
    size_t spacePos = input.find(' ');
    if (spacePos != std::string::npos) {
        cmdName = input.substr(0, spacePos);
    }

    auto command = commandRegistry_.getCommand(cmdName);
    if (command) {
        if (input == cmdName) {
            hints.push_back("          " + command->getHint());
        }
    }

    return hints;
}

}