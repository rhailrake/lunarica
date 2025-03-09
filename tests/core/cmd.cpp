#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/command_processor.h"
#include <sstream>
#include <string>

namespace lunarica {

class MockCommand : public Command {
public:
    explicit MockCommand(std::shared_ptr<Context> context) : Command(context) {}

    MOCK_METHOD1(execute, bool(const std::string& args));

    std::string getName() const override { return "mock"; }
    std::string getCategory() const override { return "test"; }
    std::string getDescription() const override { return "Mock command for testing"; }
    std::vector<std::string> getExamples() const override { return {"mock arg1 arg2"}; }
    std::string getHint() const override { return "<args>      - Mock command"; }

    std::vector<std::string> getCompletions(const std::string& input) const override {
        if (input == "a") return {"arg1", "arg2"};
        return {};
    }
};

class CommandProcessorTest : public ::testing::Test {
protected:
    std::shared_ptr<Context> context;
    std::unique_ptr<CommandProcessor> processor;
    std::stringstream outputStream;
    std::streambuf* originalCoutBuffer;
    std::shared_ptr<MockCommand> mockCommand;

    void SetUp() override {
        context = std::make_shared<Context>();
        processor = std::make_unique<CommandProcessor>(context);
        originalCoutBuffer = std::cout.rdbuf(outputStream.rdbuf());

        mockCommand = std::make_shared<MockCommand>(context);
        processor->getCommandRegistry().registerCommand(mockCommand);
    }

    void TearDown() override {
        std::cout.rdbuf(originalCoutBuffer);
    }

    std::string getOutput() {
        std::string result = outputStream.str();
        outputStream.str("");
        return result;
    }
};

TEST_F(CommandProcessorTest, ProcessValidCommand) {
    EXPECT_CALL(*mockCommand, execute(testing::Eq("arg1 arg2")))
        .WillOnce(testing::Return(true));

    bool result = processor->process("mock arg1 arg2");

    EXPECT_TRUE(result);
}

TEST_F(CommandProcessorTest, ProcessUnknownCommand) {
    bool result = processor->process("unknown_cmd arg1 arg2");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Unknown command") != std::string::npos);
    EXPECT_TRUE(result);
}

TEST_F(CommandProcessorTest, ProcessEmptyCommand) {
    bool result = processor->process("");

    EXPECT_TRUE(result);
}

TEST_F(CommandProcessorTest, GetCompletions) {
    int contextLen = 0;
    auto completions = processor->getCompletions("m", contextLen);

    EXPECT_GT(completions.size(), 0);
    bool foundMock = false;
    for (const auto& completion : completions) {
        if (completion.text() == "mock") {
            foundMock = true;
            break;
        }
    }
    EXPECT_TRUE(foundMock);
}

TEST_F(CommandProcessorTest, GetCompletionsWithArgs) {
    int contextLen = 0;
    auto completions = processor->getCompletions("mock a", contextLen);

    EXPECT_GT(completions.size(), 0);
    bool foundArg1 = false;
    bool foundArg2 = false;
    for (const auto& completion : completions) {
        if (completion.text() == "mock arg1") foundArg1 = true;
        if (completion.text() == "mock arg2") foundArg2 = true;
    }
    EXPECT_TRUE(foundArg1);
    EXPECT_TRUE(foundArg2);
}

TEST_F(CommandProcessorTest, GetHint) {
    int contextLen = 0;
    replxx::Replxx::Color color;
    auto hints = processor->getHint("mock", contextLen, color);

    EXPECT_GT(hints.size(), 0);
    EXPECT_TRUE(hints[0].find("<args>") != std::string::npos);
}

TEST_F(CommandProcessorTest, CommandCaseInsensitivity) {
    EXPECT_CALL(*mockCommand, execute(testing::Eq("arg")))
        .WillOnce(testing::Return(true));

    bool result = processor->process("MOCK arg");

    EXPECT_TRUE(result);
}

TEST_F(CommandProcessorTest, ParseCommandLine) {
    auto [cmdName, args] = processor->parseCommandLine("test  with  multiple   spaces");

    EXPECT_EQ(cmdName, "test");
    EXPECT_EQ(args, "with  multiple   spaces");
}

TEST_F(CommandProcessorTest, HighlightSyntax) {
    std::string input = "mock arg1:value";
    replxx::Replxx::colors_t colors(input.size(), replxx::Replxx::Color::DEFAULT);

    processor->highlightSyntax(input, colors);

    EXPECT_NE(colors[0], replxx::Replxx::Color::DEFAULT);
    EXPECT_NE(colors[1], replxx::Replxx::Color::DEFAULT);
    EXPECT_NE(colors[2], replxx::Replxx::Color::DEFAULT);
    EXPECT_NE(colors[3], replxx::Replxx::Color::DEFAULT);
}

}