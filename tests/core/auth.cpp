#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "commands/auth/auth_commands.h"
#include <sstream>

namespace lunarica {

class AuthCommandTest : public ::testing::Test {
protected:
    std::shared_ptr<Context> context;
    std::unique_ptr<AuthCommand> authCommand;
    std::stringstream outputStream;
    std::streambuf* originalCoutBuffer;

    void SetUp() override {
        context = std::make_shared<Context>();
        authCommand = std::make_unique<AuthCommand>(context);
        originalCoutBuffer = std::cout.rdbuf(outputStream.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(originalCoutBuffer);
    }
};

TEST_F(AuthCommandTest, BasicAuthCreatesProperHeader) {
    authCommand->execute("basic testuser password123");

    const auto& headers = context->getHeaders();
    ASSERT_TRUE(headers.find("Authorization") != headers.end());

    std::string expectedValue = "Basic " + base64_encode(std::string("testuser:password123"));
    EXPECT_EQ(headers.at("Authorization"), expectedValue);
}

TEST_F(AuthCommandTest, BearerAuthCreatesProperHeader) {
    authCommand->execute("bearer token123ABC");

    const auto& headers = context->getHeaders();
    ASSERT_TRUE(headers.find("Authorization") != headers.end());
    EXPECT_EQ(headers.at("Authorization"), "Bearer token123ABC");
}

TEST_F(AuthCommandTest, ApiKeyCreatesHeader) {
    authCommand->execute("apikey X-API-KEY my-secret-key");

    const auto& headers = context->getHeaders();
    ASSERT_TRUE(headers.find("X-API-KEY") != headers.end());
    EXPECT_EQ(headers.at("X-API-KEY"), "my-secret-key");
}

TEST_F(AuthCommandTest, ApiKeyWithMissingArgs) {
    outputStream.str("");
    authCommand->execute("apikey");

    std::string output = outputStream.str();
    EXPECT_TRUE(output.find("Usage:") != std::string::npos);

    const auto& headers = context->getHeaders();
    EXPECT_TRUE(headers.empty());
}

TEST_F(AuthCommandTest, BasicAuthWithMissingPassword) {
    outputStream.str("");
    authCommand->execute("basic username");

    std::string output = outputStream.str();
    EXPECT_TRUE(output.find("Both username and password are required") != std::string::npos);

    const auto& headers = context->getHeaders();
    EXPECT_TRUE(headers.empty());
}

TEST_F(AuthCommandTest, UnknownAuthMethod) {
    outputStream.str("");
    authCommand->execute("unknown method");

    std::string output = outputStream.str();
    EXPECT_TRUE(output.find("Unknown authentication method") != std::string::npos);
}

}