#pragma once

#include <iostream>
#include <memory>
#include <replxx.hxx>
#include <fmt/core.h>
#include "context.h"
#include "core/command_processor.h"

namespace lunarica {

    class Application {
    public:
        Application();
        ~Application() = default;

        int run();

    private:
        std::shared_ptr<Context> context_;
        std::unique_ptr<CommandProcessor> commandProcessor_;
        replxx::Replxx rx_;

        void setupReplxx();
    };

}