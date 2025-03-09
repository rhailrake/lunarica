#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "utils/encoding_utils.h"
#include <fstream>
#include <string>
#include <vector>

namespace lunarica {

class EncodingUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
        for (const auto& file : tempFiles) {
            std::remove(file.c_str());
        }
    }

    std::string createTempFileWithContent(const std::vector<char>& content, const std::string& filename = "temp_test_file.txt") {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.write(content.data(), content.size());
            file.close();
            tempFiles.push_back(filename);
            return filename;
        }
        return "";
    }

    std::vector<std::string> tempFiles;
};

TEST_F(EncodingUtilsTest, DetectUTF8Encoding) {
    std::vector<char> utf8BOM = {
        (char)0xEF, (char)0xBB, (char)0xBF, 'H', 'e', 'l', 'l', 'o'
    };
    std::string filename = createTempFileWithContent(utf8BOM);

    EXPECT_EQ(EncodingUtils::detectFileEncoding(filename), Encoding::UTF8);
}

TEST_F(EncodingUtilsTest, DetectUTF16LEEncoding) {
    std::vector<char> utf16leBOM = {
        (char)0xFF, (char)0xFE, 'H', 0x00, 'e', 0x00, 'l', 0x00, 'l', 0x00, 'o', 0x00
    };
    std::string filename = createTempFileWithContent(utf16leBOM);

    EXPECT_EQ(EncodingUtils::detectFileEncoding(filename), Encoding::UTF16LE);
}

TEST_F(EncodingUtilsTest, DetectUTF16BEEncoding) {
    std::vector<char> utf16beBOM = {
        (char)0xFE, (char)0xFF, 0x00, 'H', 0x00, 'e', 0x00, 'l', 0x00, 'l', 0x00, 'o'
    };
    std::string filename = createTempFileWithContent(utf16beBOM);

    EXPECT_EQ(EncodingUtils::detectFileEncoding(filename), Encoding::UTF16BE);
}

TEST_F(EncodingUtilsTest, DetectUTF32LEEncoding) {
    std::vector<char> utf32leBOM = {
        (char)0xFF, (char)0xFE, 0x00, 0x00, 'H', 0x00, 0x00, 0x00, 'e', 0x00, 0x00, 0x00
    };
    std::string filename = createTempFileWithContent(utf32leBOM);

    EXPECT_EQ(EncodingUtils::detectFileEncoding(filename), Encoding::UTF32LE);
}

TEST_F(EncodingUtilsTest, DetectUTF32BEEncoding) {
    std::vector<char> utf32beBOM = {
        0x00, 0x00, (char)0xFE, (char)0xFF, 0x00, 0x00, 0x00, 'H', 0x00, 0x00, 0x00, 'e'
    };
    std::string filename = createTempFileWithContent(utf32beBOM);

    EXPECT_EQ(EncodingUtils::detectFileEncoding(filename), Encoding::UTF32BE);
}

TEST_F(EncodingUtilsTest, DetectUnknownEncoding) {
    std::vector<char> noSpecificBOM = {
        'H', 'e', 'l', 'l', 'o'
    };
    std::string filename = createTempFileWithContent(noSpecificBOM);

    EXPECT_EQ(EncodingUtils::detectFileEncoding(filename), Encoding::Unknown);
}

TEST_F(EncodingUtilsTest, ReadUTF8FileToUTF8) {
    std::vector<char> utf8Content = {
        (char)0xEF, (char)0xBB, (char)0xBF, 'H', 'e', 'l', 'l', 'o'
    };
    std::string filename = createTempFileWithContent(utf8Content);

    std::string content;
    bool result = EncodingUtils::readFileToUTF8(filename, content);

    EXPECT_TRUE(result);
    EXPECT_EQ(content, std::string("Hello"));
}

TEST_F(EncodingUtilsTest, ReadUTF16LEFileToUTF8) {
    std::vector<char> utf16leContent = {
        (char)0xFF, (char)0xFE, 'H', 0x00, 'e', 0x00, 'l', 0x00, 'l', 0x00, 'o', 0x00
    };
    std::string filename = createTempFileWithContent(utf16leContent);

    std::string content;
    bool result = EncodingUtils::readFileToUTF8(filename, content);

    EXPECT_TRUE(result);
    EXPECT_EQ(content, std::string("Hello"));
}

TEST_F(EncodingUtilsTest, ReadNonexistentFile) {
    std::string content;
    bool result = EncodingUtils::readFileToUTF8("nonexistent_file.txt", content);

    EXPECT_FALSE(result);
}

TEST_F(EncodingUtilsTest, ReadEmptyFile) {
    std::vector<char> emptyContent = {};
    std::string filename = createTempFileWithContent(emptyContent);

    std::string content;
    bool result = EncodingUtils::readFileToUTF8(filename, content);

    EXPECT_TRUE(result);
    EXPECT_TRUE(content.empty());
}

}