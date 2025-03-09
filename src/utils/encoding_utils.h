#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>
#include <utf8.h>
#include <iostream>

namespace lunarica {
    enum class Encoding {
        Unknown,
        UTF8,
        UTF16LE,
        UTF16BE,
        UTF32LE,
        UTF32BE
    };

    class EncodingUtils {
    public:
        static Encoding detectFileEncoding(const std::string &filename) {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                return Encoding::Unknown;
            }

            char bom[4] = {0};
            file.read(bom, 4);
            size_t read = file.gcount();
            file.close();

            if (read >= 3 &&
                static_cast<unsigned char>(bom[0]) == 0xEF &&
                static_cast<unsigned char>(bom[1]) == 0xBB &&
                static_cast<unsigned char>(bom[2]) == 0xBF) {
                return Encoding::UTF8;
            } else if (read >= 2 &&
                       static_cast<unsigned char>(bom[0]) == 0xFF &&
                       static_cast<unsigned char>(bom[1]) == 0xFE) {
                if (read >= 4 && bom[2] == 0 && bom[3] == 0) {
                    return Encoding::UTF32LE;
                }
                return Encoding::UTF16LE;
            } else if (read >= 2 &&
                       static_cast<unsigned char>(bom[0]) == 0xFE &&
                       static_cast<unsigned char>(bom[1]) == 0xFF) {
                return Encoding::UTF16BE;
            } else if (read >= 4 &&
                       static_cast<unsigned char>(bom[0]) == 0 &&
                       static_cast<unsigned char>(bom[1]) == 0 &&
                       static_cast<unsigned char>(bom[2]) == 0xFE &&
                       static_cast<unsigned char>(bom[3]) == 0xFF) {
                return Encoding::UTF32BE;
            }

            return Encoding::Unknown;
        }

        static bool readFileToUTF8(const std::string &filename, std::string &content,
                                   Encoding forcedEncoding = Encoding::Unknown) {
            Encoding encoding = forcedEncoding;
            if (encoding == Encoding::Unknown) {
                encoding = detectFileEncoding(filename);
            }

            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                return false;
            }

            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            if (fileSize == 0) {
                content.clear();
                return true;
            }

            if (encoding == Encoding::UTF8) {
                char bom[3];
                file.read(bom, 3);
                if (!(static_cast<unsigned char>(bom[0]) == 0xEF &&
                      static_cast<unsigned char>(bom[1]) == 0xBB &&
                      static_cast<unsigned char>(bom[2]) == 0xBF)) {
                    file.seekg(0, std::ios::beg);
                } else {
                    fileSize -= 3;
                }
            } else if (encoding == Encoding::UTF16LE || encoding == Encoding::UTF16BE) {
                char bom[2];
                file.read(bom, 2);
                fileSize -= 2;
            } else if (encoding == Encoding::UTF32LE || encoding == Encoding::UTF32BE) {
                char bom[4];
                file.read(bom, 4);
                fileSize -= 4;
            }

            switch (encoding) {
                case Encoding::UTF8:
                case Encoding::Unknown: {
                    std::vector<char> buffer(fileSize);
                    file.read(buffer.data(), fileSize);
                    content.assign(buffer.data(), fileSize);
                    break;
                }

                case Encoding::UTF16LE: {
                    std::vector<char> buffer(fileSize);
                    file.read(buffer.data(), fileSize);

                    std::u16string utf16String;
                    for (size_t i = 0; i < fileSize; i += 2) {
                        if (i + 1 < fileSize) {
                            char16_t c = static_cast<char16_t>(
                                (static_cast<unsigned char>(buffer[i])) |
                                (static_cast<unsigned char>(buffer[i + 1]) << 8)
                            );
                            utf16String.push_back(c);
                        }
                    }

                    content.clear();
                    utf8::utf16to8(utf16String.begin(), utf16String.end(), std::back_inserter(content));
                    break;
                }

                case Encoding::UTF16BE: {
                    std::vector<char> buffer(fileSize);
                    file.read(buffer.data(), fileSize);

                    std::u16string utf16String;
                    for (size_t i = 0; i < fileSize; i += 2) {
                        if (i + 1 < fileSize) {
                            char16_t c = static_cast<char16_t>(
                                (static_cast<unsigned char>(buffer[i]) << 8) |
                                (static_cast<unsigned char>(buffer[i + 1]))
                            );
                            utf16String.push_back(c);
                        }
                    }

                    content.clear();
                    utf8::utf16to8(utf16String.begin(), utf16String.end(), std::back_inserter(content));
                    break;
                }

                case Encoding::UTF32LE: {
                    std::vector<char> buffer(fileSize);
                    file.read(buffer.data(), fileSize);

                    std::u32string utf32String;
                    for (size_t i = 0; i < fileSize; i += 4) {
                        if (i + 3 < fileSize) {
                            char32_t c = static_cast<char32_t>(
                                (static_cast<unsigned char>(buffer[i])) |
                                (static_cast<unsigned char>(buffer[i + 1]) << 8) |
                                (static_cast<unsigned char>(buffer[i + 2]) << 16) |
                                (static_cast<unsigned char>(buffer[i + 3]) << 24)
                            );
                            utf32String.push_back(c);
                        }
                    }

                    content.clear();
                    utf8::utf32to8(utf32String.begin(), utf32String.end(), std::back_inserter(content));
                    break;
                }

                case Encoding::UTF32BE: {
                    std::vector<char> buffer(fileSize);
                    file.read(buffer.data(), fileSize);

                    std::u32string utf32String;
                    for (size_t i = 0; i < fileSize; i += 4) {
                        if (i + 3 < fileSize) {
                            char32_t c = static_cast<char32_t>(
                                (static_cast<unsigned char>(buffer[i]) << 24) |
                                (static_cast<unsigned char>(buffer[i + 1]) << 16) |
                                (static_cast<unsigned char>(buffer[i + 2]) << 8) |
                                (static_cast<unsigned char>(buffer[i + 3]))
                            );
                            utf32String.push_back(c);
                        }
                    }

                    content.clear();
                    utf8::utf32to8(utf32String.begin(), utf32String.end(), std::back_inserter(content));
                    break;
                }
            }

            return true;
        }

        static std::string getEncodingName(Encoding encoding) {
            switch (encoding) {
                case Encoding::UTF8: return "UTF-8";
                case Encoding::UTF16LE: return "UTF-16 LE";
                case Encoding::UTF16BE: return "UTF-16 BE";
                case Encoding::UTF32LE: return "UTF-32 LE";
                case Encoding::UTF32BE: return "UTF-32 BE";
                default: return "Unknown";
            }
        }
    };
}
