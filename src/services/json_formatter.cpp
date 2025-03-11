#include "json_formatter.h"

namespace lunarica {

void JsonFormatter::format(const std::string& json) {
    try {
        Json::Value parsedJson;
        Json::CharReaderBuilder builder;
        builder["collectComments"] = false;

        std::string errs;
        std::istringstream jsonStream(json);
        bool parseSuccess = Json::parseFromStream(builder, jsonStream, &parsedJson, &errs);

        if (parseSuccess) {
            Json::StyledWriter styledWriter;
            std::string formattedJson = styledWriter.write(parsedJson);
            highlightAndPrintJson(formattedJson);
        } else {
            std::cout << json << std::endl;
        }
    } catch (const std::exception&) {
        std::cout << json << std::endl;
    }
}

void JsonFormatter::highlightAndPrintJson(const std::string& json) {
    const std::string COLOR_RESET = "\033[0m";
    const std::string COLOR_KEY = "\033[38;5;208m";
    const std::string COLOR_STRING = "\033[1;32m";
    const std::string COLOR_NUMBER = "\033[1;33m";
    const std::string COLOR_BOOL = "\033[1;35m";
    const std::string COLOR_NULL = "\033[1;31m";
    const std::string COLOR_BRACKET = "\033[1;37m";
    const std::string COLOR_COMMA = "\033[1;37m";
    const std::string COLOR_COLON = "\033[1;37m";

    std::string result;
    bool inString = false;
    bool escapeNext = false;
    std::stack<char> brackets;
    int indentLevel = 0;
    int terminalWidth = getTerminalWidth();

    for (size_t i = 0; i < json.length(); ++i) {
        char c = json[i];

        if (inString) {
            if (escapeNext) {
                result += c;
                escapeNext = false;
                continue;
            }

            if (c == '\\') {
                result += c;
                escapeNext = true;
                continue;
            }

            if (c == '"') {
                inString = false;
                result += COLOR_STRING + "\"" + COLOR_RESET;
                continue;
            }

            result += c;
            continue;
        }

        switch (c) {
            case '"': {
                inString = true;

                size_t j = i + 1;
                bool foundClosingQuote = false;
                bool isKey = false;

                while (j < json.length() && !foundClosingQuote) {
                    if (json[j] == '\\') {
                        j += 2;
                        continue;
                    }

                    if (json[j] == '"') {
                        foundClosingQuote = true;

                        size_t k = j + 1;
                        while (k < json.length() && std::isspace(json[k])) {
                            k++;
                        }

                        if (k < json.length() && json[k] == ':') {
                            isKey = true;
                        }
                    }
                    j++;
                }

                result += isKey ? COLOR_KEY + "\"" : COLOR_STRING + "\"";
                break;
            }
            case '{':
            case '[': {
                brackets.push(c);
                indentLevel++;
                result += COLOR_BRACKET + c + COLOR_RESET + "\n" + std::string(indentLevel * 2, ' ');
                break;
            }
            case '}':
            case ']': {
                if (!brackets.empty()) {
                    brackets.pop();
                }
                indentLevel = std::max(0, indentLevel - 1);
                result += "\n" + std::string(indentLevel * 2, ' ') + COLOR_BRACKET + c + COLOR_RESET;
                break;
            }
            case ':': {
                result += COLOR_COLON + c + COLOR_RESET + " ";
                break;
            }
            case ',': {
                result += COLOR_COMMA + c + COLOR_RESET + "\n" + std::string(indentLevel * 2, ' ');
                break;
            }
            case 't':
            case 'f': {
                if (i + 3 < json.length() && json.substr(i, 4) == "true") {
                    result += COLOR_BOOL + "true" + COLOR_RESET;
                    i += 3;
                } else if (i + 4 < json.length() && json.substr(i, 5) == "false") {
                    result += COLOR_BOOL + "false" + COLOR_RESET;
                    i += 4;
                } else {
                    result += c;
                }
                break;
            }
            case 'n': {
                if (i + 3 < json.length() && json.substr(i, 4) == "null") {
                    result += COLOR_NULL + "null" + COLOR_RESET;
                    i += 3;
                } else {
                    result += c;
                }
                break;
            }
            default: {
                if (std::isdigit(c) || c == '-' || c == '+' || c == '.') {
                    std::string number;
                    size_t j = i;

                    while (j < json.length() &&
                           (std::isdigit(json[j]) || json[j] == '-' || json[j] == '+' ||
                            json[j] == '.' || json[j] == 'e' || json[j] == 'E')) {
                        number += json[j];
                        j++;
                    }

                    if (isNumber(number)) {
                        result += COLOR_NUMBER + number + COLOR_RESET;
                        i = j - 1;
                    } else {
                        result += c;
                    }
                } else if (std::isspace(c)) {
                    if (c == '\n') {
                        result += "\n" + std::string(indentLevel * 2, ' ');
                    }
                } else {
                    result += c;
                }
                break;
            }
        }
    }

    std::istringstream resultStream(result);
    std::string line;

    while (std::getline(resultStream, line)) {
        size_t contentWidth = stripANSI(line).length();

        if (contentWidth > static_cast<size_t>(terminalWidth - 5)) {
            printWrappedLine(line, countLeadingSpaces(line), terminalWidth);
        } else {
            std::cout << line << std::endl;
        }
    }
}

bool JsonFormatter::isNumber(const std::string& s) {
    if (s.empty()) return false;

    char* end = nullptr;
    std::strtod(s.c_str(), &end);

    return end != s.c_str() && *end == '\0' &&
           s.find_first_not_of("-+.eE0123456789") == std::string::npos;
}

bool JsonFormatter::isValidNumberChar(char c, char prev) {
    if (std::isdigit(c)) {
        return true;
    }

    switch (c) {
        case '.':
            return prev != '.';
        case 'e':
        case 'E':
            return true;
        case '+':
        case '-':
            return prev == 'e' || prev == 'E';
        default:
            return false;
    }
}

size_t JsonFormatter::countLeadingSpaces(const std::string& s) {
    size_t count = 0;
    while (count < s.size() && std::isspace(s[count])) {
        count++;
    }
    return count;
}

std::string JsonFormatter::trimLeft(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(s[start])) {
        start++;
    }
    return s.substr(start);
}

int JsonFormatter::getTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns = 80;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return columns;
#else
    struct winsize w;
    if (ioctl(fileno(stdout), TIOCGWINSZ, &w) != -1) {
        return w.ws_col;
    }
    return 80;
#endif
}

void JsonFormatter::printWrappedLine(const std::string& line, size_t indent, int maxWidth) {
    const int indentWidth = static_cast<int>(indent * 2);
    const int contentWidth = maxWidth - indentWidth - 5;

    size_t pos = 0;
    while (pos < line.length()) {
        size_t chunkSize = std::min(static_cast<size_t>(contentWidth), line.length() - pos);

        if (chunkSize < line.length() - pos) {
            size_t breakPos = pos + chunkSize;
            while (breakPos > pos && !std::isspace(line[breakPos])) {
                breakPos--;
            }

            if (breakPos == pos) {
                breakPos = pos + chunkSize;
            } else {
                chunkSize = breakPos - pos;
            }
        }

        std::cout << std::string(indentWidth, ' ') << line.substr(pos, chunkSize) << std::endl;
        pos += chunkSize;

        while (pos < line.length() && std::isspace(line[pos])) {
            pos++;
        }
    }
}

std::string JsonFormatter::stripANSI(const std::string& input) {
    std::string result;
    bool inEscapeCode = false;

    for (char c : input) {
        if (c == '\033') {
            inEscapeCode = true;
            continue;
        }

        if (inEscapeCode) {
            if (c == 'm') {
                inEscapeCode = false;
            }
            continue;
        }

        result += c;
    }

    return result;
}

}