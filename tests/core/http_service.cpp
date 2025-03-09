#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "services/http_service.h"
#include "utils/encoding_utils.h"
#include <memory>
#include <string>

namespace lunarica {

class HttpServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<Context> context;
    std::shared_ptr<JsonFormatter> formatter;
    std::unique_ptr<HttpService> httpService;
    std::streambuf* originalCoutBuffer;
    std::ostringstream outputStream;

    void SetUp() override {
        context = std::make_shared<Context>();
        formatter = std::make_shared<JsonFormatter>();
        httpService = std::make_unique<HttpService>(context, formatter);
        originalCoutBuffer = std::cout.rdbuf(outputStream.rdbuf());
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

TEST_F(HttpServiceTest, JSONPlaceholderGetTest) {
    context->setUrl("https://jsonplaceholder.typicode.com");

    httpService->get("/posts/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
    EXPECT_TRUE(output.find("application/json") != std::string::npos);
}

TEST_F(HttpServiceTest, JSONPlaceholderPostTest) {
    context->setUrl("https://jsonplaceholder.typicode.com");
    context->addBodyParam("title", "Test Title");
    context->addBodyParam("body", "Test Body");
    context->addBodyParam("userId", "1");

    httpService->post("/posts");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making POST request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 201") != std::string::npos);
}

TEST_F(HttpServiceTest, JSONPlaceholderPutTest) {
    context->setUrl("https://jsonplaceholder.typicode.com");
    context->addBodyParam("id", "1");
    context->addBodyParam("title", "Updated Title");
    context->addBodyParam("body", "Updated Body");
    context->addBodyParam("userId", "1");

    httpService->put("/posts/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making PUT request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
}

TEST_F(HttpServiceTest, JSONPlaceholderDeleteTest) {
    context->setUrl("https://jsonplaceholder.typicode.com");

    httpService->del("/posts/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making DELETE request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
}

TEST_F(HttpServiceTest, QueryParametersTest) {
    context->setUrl("https://httpbin.org");
    context->addQueryParam("param1", "value1");
    context->addQueryParam("param2", "value2");

    httpService->get("/get");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
    EXPECT_TRUE(output.find("param1=value1") != std::string::npos ||
                output.find("param2=value2") != std::string::npos);
}

TEST_F(HttpServiceTest, CustomHeadersTest) {
    context->setUrl("https://httpbin.org");
    context->addHeader("X-Custom-Header", "custom-value");
    context->addHeader("Accept", "application/json");

    httpService->get("/headers");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
}

TEST_F(HttpServiceTest, JSONDataBodyTest) {
    context->setUrl("https://httpbin.org");
    context->addBodyParam("string", "value");
    context->addBodyParam("number", "42");
    context->addBodyParam("boolean", "true");
    context->addBodyParam("null_value", "null");

    httpService->post("/post");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making POST request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
}

TEST_F(HttpServiceTest, URLParsingTest) {
    context->setUrl("http://localhost:8000");

    httpService->get("/api/test");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request to: http://localhost:8000/api/test") != std::string::npos);
}

TEST_F(HttpServiceTest, RequestBodyJSONFormattingTest) {
    context->setUrl("https://httpbin.org");
    context->addBodyParam("string", "value");
    context->addBodyParam("number", "42");
    context->addBodyParam("boolean", "true");
    context->addBodyParam("null_value", "null");

    httpService->post("/post");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making POST request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
}

TEST_F(HttpServiceTest, ContextTimeoutSettingsTest) {
    context->setUrl("https://httpbin.org");

    context->setConnectionTimeout(10);
    EXPECT_EQ(10, context->getConnectionTimeout());

    context->setReadTimeout(15);
    EXPECT_EQ(15, context->getReadTimeout());
}

TEST_F(HttpServiceTest, AbsolutePathTest) {
    context->setUrl("https://jsonplaceholder.typicode.com/posts");

    httpService->get("/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request to: https://jsonplaceholder.typicode.com/1") != std::string::npos);
}

TEST_F(HttpServiceTest, RelativePathTest) {
    context->setUrl("https://jsonplaceholder.typicode.com/posts");

    httpService->get("1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request to: https://jsonplaceholder.typicode.com/posts/1") != std::string::npos);
}

TEST_F(HttpServiceTest, CompleteURLTest) {
    context->setUrl("https://example.com");

    httpService->get("https://jsonplaceholder.typicode.com/posts/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request to: https://jsonplaceholder.typicode.com/posts/1") != std::string::npos);
}

}