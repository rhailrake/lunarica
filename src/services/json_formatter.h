#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <json/json.h>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace lunarica {

    class JsonFormatter {
    public:
        JsonFormatter() = default;
        ~JsonFormatter() = default;

        void format(const std::string& json);

    private:
        void highlightAndPrintJson(const std::string& json);
        bool isNumber(const std::string& s);
        bool isValidNumberChar(char c, char prev);
        size_t countLeadingSpaces(const std::string& s);
        std::string trimLeft(const std::string& s);
        int getTerminalWidth();
        void printWrappedLine(const std::string& line, size_t indent, int maxWidth);
        std::string stripANSI(const std::string& input);
    };

}