#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <replxx.hxx>
#include <sstream>
#include <string>
#include "command_registry.h"
#include "context.h"
#include "services/http_service.h"
#include "services/json_formatter.h"
#include "commands/auth/auth_commands.h"
#include "commands/body/body_commands.h"
#include "commands/headers/header_commands.h"
#include "commands/misc/misc_commands.h"
#include "commands/network/http_commands.h"
#include "commands/query/query_commands.h"
#include "commands/system/cd_command.h"
#include "commands/system/exit_command.h"
#include "commands/system/help_command.h"

namespace lunarica {

    class CommandProcessor {
    public:
        explicit CommandProcessor(std::shared_ptr<Context> context);
        ~CommandProcessor() = default;

        bool process(const std::string& commandLine);

        std::vector<replxx::Replxx::Completion> getCompletions(
            const std::string& input, int& context_len);

        void highlightSyntax(const std::string& input,
                            replxx::Replxx::colors_t& colors);

        std::vector<std::string> getHint(const std::string& input,
                                        int& context_len,
                                        replxx::Replxx::Color& color);

    private:
        std::shared_ptr<Context> context_;
        std::shared_ptr<JsonFormatter> jsonFormatter_;
        std::shared_ptr<HttpService> httpService_;
        CommandRegistry commandRegistry_;

        void registerCommands();

        std::pair<std::string, std::string> parseCommandLine(const std::string& commandLine);
    };

}