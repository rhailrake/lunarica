#include "services/http_service.h"

#include <memory>
#include <string>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../utils/test_http_server.h"
#include "utils/encoding_utils.h"

namespace lunarica {

class HttpServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<Context> context;
    std::shared_ptr<JsonFormatter> formatter;
    std::unique_ptr<HttpService> httpService;
    std::unique_ptr<testing::TestHttpServer> testServer;
    std::streambuf* originalCoutBuffer;
    std::ostringstream outputStream;

    void SetUp() override {
        context = std::make_shared<Context>();
        formatter = std::make_shared<JsonFormatter>();
        httpService = std::make_unique<HttpService>(context, formatter);

        testServer = std::make_unique<testing::TestHttpServer>(8090);
        testServer->start();

        context->setUrl(testServer->getBaseUrl());

        originalCoutBuffer = std::cout.rdbuf(outputStream.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(originalCoutBuffer);
        testServer->stop();
    }

    std::string getOutput() {
        std::string result = outputStream.str();
        outputStream.str("");
        return result;
    }
};

TEST_F(HttpServiceTest, GetRequestTest) {
    httpService->get("/posts/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
    EXPECT_TRUE(output.find("Content-Type: application/json") != std::string::npos);
    EXPECT_TRUE(output.find("Test Post") != std::string::npos);
}

TEST_F(HttpServiceTest, PostRequestTest) {
    context->addBodyParam("title", "New Post");
    context->addBodyParam("body", "Content");
    context->addBodyParam("userId", "1");

    httpService->post("/posts");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making POST request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 201") != std::string::npos);
    EXPECT_TRUE(output.find("id") != std::string::npos);
    EXPECT_TRUE(output.find("New Post") != std::string::npos);
}

TEST_F(HttpServiceTest, PutRequestTest) {
    context->addBodyParam("title", "Updated Post");
    context->addBodyParam("body", "Updated Content");
    context->addBodyParam("userId", "1");

    httpService->put("/posts/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making PUT request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
    EXPECT_TRUE(output.find("Updated Post") != std::string::npos);
    EXPECT_TRUE(output.find("Updated Content") != std::string::npos);
}

TEST_F(HttpServiceTest, DeleteRequestTest) {
    httpService->del("/posts/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making DELETE request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
}

TEST_F(HttpServiceTest, QueryParametersTest) {
    context->addQueryParam("param1", "value1");
    context->addQueryParam("param2", "value2");

    httpService->get("/query");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
    EXPECT_TRUE(output.find("param1") != std::string::npos);
    EXPECT_TRUE(output.find("value1") != std::string::npos);
    EXPECT_TRUE(output.find("param2") != std::string::npos);
    EXPECT_TRUE(output.find("value2") != std::string::npos);
}

TEST_F(HttpServiceTest, CustomHeadersTest) {
    context->addHeader("X-Custom-Header", "custom-value");

    httpService->get("/headers");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 200") != std::string::npos);
    EXPECT_TRUE(output.find("X-Custom-Header") != std::string::npos);
    EXPECT_TRUE(output.find("custom-value") != std::string::npos);
}

TEST_F(HttpServiceTest, NotFoundTest) {
    httpService->get("/not-found");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request") != std::string::npos);
    EXPECT_TRUE(output.find("STATUS: 404") != std::string::npos);
    EXPECT_TRUE(output.find("Not Found") != std::string::npos);
}

TEST_F(HttpServiceTest, URLParsingTest) {
    std::string baseUrl = testServer->getBaseUrl();
    context->setUrl(baseUrl);

    httpService->get("/posts/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request to: " + baseUrl + "/posts/1") != std::string::npos);
}

TEST_F(HttpServiceTest, ContextTimeoutSettingsTest) {
    context->setConnectionTimeout(10);
    EXPECT_EQ(10, context->getConnectionTimeout());

    context->setReadTimeout(15);
    EXPECT_EQ(15, context->getReadTimeout());
}

TEST_F(HttpServiceTest, AbsolutePathTest) {
    std::string baseUrl = testServer->getBaseUrl();
    context->setUrl(baseUrl + "/posts");

    httpService->get("/1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request to: " + baseUrl + "/1") != std::string::npos);
}

TEST_F(HttpServiceTest, RelativePathTest) {
    std::string baseUrl = testServer->getBaseUrl();
    context->setUrl(baseUrl + "/posts");

    httpService->get("1");

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request to: " + baseUrl + "/posts/1") != std::string::npos);
}

TEST_F(HttpServiceTest, CompleteURLTest) {
    context->setUrl("http://example.com");

    std::string testUrl = testServer->getBaseUrl() + "/posts/1";
    httpService->get(testUrl);

    std::string output = getOutput();
    EXPECT_TRUE(output.find("Making GET request to: " + testUrl) != std::string::npos);
}

}
