#include "json_formatter.h"

namespace lunarica {

void JsonFormatter::format(const std::string& json) {
    try {
        auto parsedJson = nlohmann::json::parse(json);
        std::string formattedJson = parsedJson.dump(2);
        highlightAndPrintJson(formattedJson);
    } catch (const nlohmann::json::exception&) {
        std::cout << json << std::endl;
    }
}

void JsonFormatter::highlightAndPrintJson(const std::string& json) {
    const std::string COLOR_RESET = "\033[0m";
    const std::string COLOR_KEY = "\033[38;5;208m";    // Orange for keys
    const std::string COLOR_STRING = "\033[1;32m";     // Green for strings
    const std::string COLOR_NUMBER = "\033[1;33m";     // Yellow for numbers
    const std::string COLOR_BOOL = "\033[1;35m";       // Purple for booleans
    const std::string COLOR_NULL = "\033[1;31m";       // Red for null
    const std::string COLOR_BRACKET = "\033[1;37m";    // White for brackets/braces
    const std::string COLOR_COMMA = "\033[1;37m";      // White for commas
    const std::string COLOR_COLON = "\033[1;37m";      // White for colons

    enum class TokenType {
        String, Key, Number, Boolean, Null, Bracket, Colon, Comma, Whitespace, Unknown
    };

    std::string result;
    bool inString = false;
    bool escapeNext = false;
    std::string currentToken;
    TokenType currentTokenType = TokenType::Unknown;
    int terminalWidth = getTerminalWidth();

    for (size_t i = 0; i < json.length(); ++i) {
        char c = json[i];

        if (inString) {
            if (escapeNext) {
                currentToken += c;
                escapeNext = false;
                continue;
            }

            if (c == '\\') {
                currentToken += c;
                escapeNext = true;
                continue;
            }

            if (c == '"') {
                currentToken += c;

                if (currentTokenType == TokenType::Key) {
                    result += COLOR_KEY + currentToken + COLOR_RESET;
                } else {
                    result += COLOR_STRING + currentToken + COLOR_RESET;
                }

                currentToken.clear();
                inString = false;
                currentTokenType = TokenType::Unknown;
                continue;
            }

            currentToken += c;
            continue;
        }

        if (c == '"') {
            if (!currentToken.empty()) {
                if (currentTokenType == TokenType::Number) {
                    result += COLOR_NUMBER + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Boolean) {
                    result += COLOR_BOOL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Null) {
                    result += COLOR_NULL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Bracket) {
                    result += COLOR_BRACKET + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Colon) {
                    result += COLOR_COLON + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Comma) {
                    result += COLOR_COMMA + currentToken + COLOR_RESET;
                } else {
                    result += currentToken;
                }
                currentToken.clear();
            }

            bool isKey = false;
            size_t j = i + 1;
            int stringEndCount = 0;

            while (j < json.length()) {
                if (stringEndCount == 0 && json[j] == '"' && (j == 0 || json[j-1] != '\\')) {
                    stringEndCount = 1;
                    j++;
                    continue;
                }

                if (stringEndCount == 1) {
                    if (json[j] == ':') {
                        isKey = true;
                        break;
                    }

                    if (!std::isspace(json[j])) {
                        break;
                    }
                }

                j++;
            }

            inString = true;
            currentToken = c;
            currentTokenType = isKey ? TokenType::Key : TokenType::String;
            continue;
        }

        if (c == '{' || c == '}' || c == '[' || c == ']') {
            if (!currentToken.empty()) {
                if (currentTokenType == TokenType::Number) {
                    result += COLOR_NUMBER + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Boolean) {
                    result += COLOR_BOOL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Null) {
                    result += COLOR_NULL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Bracket) {
                    result += COLOR_BRACKET + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Colon) {
                    result += COLOR_COLON + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Comma) {
                    result += COLOR_COMMA + currentToken + COLOR_RESET;
                } else {
                    result += currentToken;
                }
                currentToken.clear();
            }

            result += COLOR_BRACKET + c + COLOR_RESET;
            currentTokenType = TokenType::Unknown;
            continue;
        }

        if (c == ':') {
            if (!currentToken.empty()) {
                if (currentTokenType == TokenType::Number) {
                    result += COLOR_NUMBER + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Boolean) {
                    result += COLOR_BOOL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Null) {
                    result += COLOR_NULL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Bracket) {
                    result += COLOR_BRACKET + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Colon) {
                    result += COLOR_COLON + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Comma) {
                    result += COLOR_COMMA + currentToken + COLOR_RESET;
                } else {
                    result += currentToken;
                }
                currentToken.clear();
            }

            result += COLOR_COLON + c + COLOR_RESET;
            currentTokenType = TokenType::Unknown;
            continue;
        }

        if (c == ',') {
            if (!currentToken.empty()) {
                if (currentTokenType == TokenType::Number) {
                    result += COLOR_NUMBER + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Boolean) {
                    result += COLOR_BOOL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Null) {
                    result += COLOR_NULL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Bracket) {
                    result += COLOR_BRACKET + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Colon) {
                    result += COLOR_COLON + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Comma) {
                    result += COLOR_COMMA + currentToken + COLOR_RESET;
                } else {
                    result += currentToken;
                }
                currentToken.clear();
            }

            result += COLOR_COMMA + c + COLOR_RESET;
            currentTokenType = TokenType::Unknown;
            continue;
        }

        if (std::isspace(c)) {
            if (!currentToken.empty()) {
                if (currentTokenType == TokenType::Number) {
                    result += COLOR_NUMBER + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Boolean) {
                    result += COLOR_BOOL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Null) {
                    result += COLOR_NULL + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Bracket) {
                    result += COLOR_BRACKET + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Colon) {
                    result += COLOR_COLON + currentToken + COLOR_RESET;
                } else if (currentTokenType == TokenType::Comma) {
                    result += COLOR_COMMA + currentToken + COLOR_RESET;
                } else {
                    result += currentToken;
                }
                currentToken.clear();
            }

            result += c;
            currentTokenType = TokenType::Unknown;
            continue;
        }

        if (currentToken.empty()) {
            currentToken = c;

            if (c == 't' || c == 'f') {
                currentTokenType = TokenType::Boolean;
            } else if (c == 'n') {
                currentTokenType = TokenType::Null;
            } else if (std::isdigit(c) || c == '-' || c == '+') {
                currentTokenType = TokenType::Number;
            } else {
                currentTokenType = TokenType::Unknown;
            }
        } else {
            currentToken += c;

            if (currentTokenType == TokenType::Boolean) {
                if (!(currentToken == "t" || currentToken == "tr" || currentToken == "tru" || currentToken == "true" ||
                      currentToken == "f" || currentToken == "fa" || currentToken == "fal" || currentToken == "fals" || currentToken == "false")) {
                    currentTokenType = TokenType::Unknown;
                }
            } else if (currentTokenType == TokenType::Null) {
                if (!(currentToken == "n" || currentToken == "nu" || currentToken == "nul" || currentToken == "null")) {
                    currentTokenType = TokenType::Unknown;
                }
            } else if (currentTokenType == TokenType::Number) {
                if (!isValidNumberChar(c, currentToken.length() > 1 ? currentToken[currentToken.length() - 2] : '\0')) {
                    currentTokenType = TokenType::Unknown;
                }
            }
        }
    }

    if (!currentToken.empty()) {
        if (currentTokenType == TokenType::Number) {
            result += COLOR_NUMBER + currentToken + COLOR_RESET;
        } else if (currentTokenType == TokenType::Boolean) {
            result += COLOR_BOOL + currentToken + COLOR_RESET;
        } else if (currentTokenType == TokenType::Null) {
            result += COLOR_NULL + currentToken + COLOR_RESET;
        } else if (currentTokenType == TokenType::Bracket) {
            result += COLOR_BRACKET + currentToken + COLOR_RESET;
        } else if (currentTokenType == TokenType::Colon) {
            result += COLOR_COLON + currentToken + COLOR_RESET;
        } else if (currentTokenType == TokenType::Comma) {
            result += COLOR_COMMA + currentToken + COLOR_RESET;
        } else {
            result += currentToken;
        }
    }

    std::istringstream stream(result);
    std::string line;
    std::cout << std::endl;

    while (std::getline(stream, line)) {
        size_t indent = countLeadingSpaces(line);
        if (line.length() > terminalWidth - 5 && indent < terminalWidth / 2) {
            printWrappedLine(line, indent / 2, terminalWidth);
        } else {
            std::cout << line << std::endl;
        }
    }

    std::cout << std::endl;
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
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col > 0 ? w.ws_col : 80;
#endif
}

void JsonFormatter::printWrappedLine(const std::string& line, size_t indent, int maxWidth) {
    const int indentWidth = indent * 2;
    const int contentWidth = maxWidth - indentWidth - 5;

    size_t pos = 0;
    while (pos < line.length()) {
        size_t chunkSize = std::min<size_t>(contentWidth, line.length() - pos);

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

}