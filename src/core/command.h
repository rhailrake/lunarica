#pragma once

#include <string>
#include <vector>
#include <memory>
#include "context.h"

namespace lunarica {

    class Command {
    public:
        virtual ~Command() = default;

        virtual std::string getName() const = 0;

        virtual std::vector<std::string> getAliases() const {
            return {};
        }

        virtual std::string getCategory() const = 0;

        virtual std::string getDescription() const = 0;

        virtual std::vector<std::string> getExamples() const = 0;

        virtual bool execute(const std::string& args) = 0;

        virtual std::vector<std::string> getCompletions(const std::string& input) const {
            return {};
        }

        virtual std::string getHint() const = 0;

        void setContext(std::shared_ptr<Context> context) {
            context_ = context;
        }

    protected:
        std::shared_ptr<Context> context_;

        explicit Command(std::shared_ptr<Context> context = nullptr) : context_(context) {}
    };

}