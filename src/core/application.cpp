#include "application.h"

namespace lunarica {

    Application::Application()
        : context_(std::make_shared<Context>()),
          commandProcessor_(std::make_unique<CommandProcessor>(context_)) {
        setupReplxx();
    }

    void Application::setupReplxx() {
        rx_.set_max_history_size(1000);
        rx_.set_completion_callback([this](const std::string& input, int& context_len) {
            return commandProcessor_->getCompletions(input, context_len);
        });

        rx_.set_highlighter_callback([this](const std::string& input, replxx::Replxx::colors_t& colors) {
            commandProcessor_->highlightSyntax(input, colors);
        });

        rx_.set_hint_callback([this](const std::string& input, int& context_len, replxx::Replxx::Color& color) {
            return commandProcessor_->getHint(input, context_len, color);
        });

        rx_.set_word_break_characters(" \t\n");
        rx_.set_hint_delay(0);
        rx_.set_double_tab_completion(false);
        rx_.set_complete_on_empty(false);
        rx_.set_beep_on_ambiguous_completion(false);
        rx_.set_no_color(false);
        rx_.set_max_hint_rows(1);

        rx_.history_load("lunarica_history.txt");
    }

    int Application::run() {
        std::cout << "Lunarica - Interactive HTTP Client v0.1.6" << std::endl;
        std::cout << "Type 'help' for available commands, 'exit' to quit" << std::endl;

        while (!context_->shouldExit()) {
            std::string prompt = fmt::format("{} > ", context_->getUrl());

            const char* input = rx_.input(prompt);

            if (input == nullptr) {
                break;
            }

            std::string line(input);
            if (!line.empty()) {
                rx_.history_add(line);

                std::cout << std::endl;

                commandProcessor_->process(line);

                std::cout << std::endl;
            }
        }

        rx_.history_save("lunarica_history.txt");

        return 0;
    }

}