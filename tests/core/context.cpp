#include <gtest/gtest.h>
#include "core/context.h"

namespace lunarica {

TEST(ContextTest, InitialState) {
    Context context;

    EXPECT_EQ(context.getUrl(), "http://localhost:8000");
    EXPECT_FALSE(context.shouldExit());
    EXPECT_TRUE(context.getHeaders().empty());
    EXPECT_TRUE(context.getQueryParams().empty());
    EXPECT_TRUE(context.getBodyParams().empty());
    EXPECT_EQ(context.getConnectionTimeout(), 3);
    EXPECT_EQ(context.getReadTimeout(), 5);
}

TEST(ContextTest, UrlManagement) {
    Context context;

    context.setUrl("https://example.com/api");
    EXPECT_EQ(context.getUrl(), "https://example.com/api");

    context.setUrl("http://localhost:3000");
    EXPECT_EQ(context.getUrl(), "http://localhost:3000");
}

TEST(ContextTest, HeaderManagement) {
    Context context;

    context.addHeader("Content-Type", "application/json");
    context.addHeader("Authorization", "Bearer token123");

    const auto& headers = context.getHeaders();
    EXPECT_EQ(headers.size(), 2);
    EXPECT_EQ(headers.at("Content-Type"), "application/json");
    EXPECT_EQ(headers.at("Authorization"), "Bearer token123");

    bool removed = context.removeHeader("Content-Type");
    EXPECT_TRUE(removed);
    EXPECT_EQ(context.getHeaders().size(), 1);
    EXPECT_FALSE(context.getHeaders().count("Content-Type"));

    removed = context.removeHeader("NonExistent");
    EXPECT_FALSE(removed);

    context.clearHeaders();
    EXPECT_TRUE(context.getHeaders().empty());
}

TEST(ContextTest, QueryParamManagement) {
    Context context;

    context.addQueryParam("page", "1");
    context.addQueryParam("limit", "20");
    context.addQueryParam("sort", "name");
    context.addQueryParam("sort", "age");

    const auto& params = context.getQueryParams();
    EXPECT_EQ(params.size(), 3);
    EXPECT_EQ(params.at("page").size(), 1);
    EXPECT_EQ(params.at("page")[0], "1");
    EXPECT_EQ(params.at("sort").size(), 2);

    bool removed = context.removeQueryParam("limit");
    EXPECT_TRUE(removed);
    EXPECT_EQ(context.getQueryParams().size(), 2);

    removed = context.removeQueryParam("NonExistent");
    EXPECT_FALSE(removed);

    context.clearQueryParams();
    EXPECT_TRUE(context.getQueryParams().empty());
}

TEST(ContextTest, BodyParamManagement) {
    Context context;

    context.addBodyParam("username", "john");
    context.addBodyParam("age", "30");
    context.addBodyParam("active", "true");

    const auto& params = context.getBodyParams();
    EXPECT_EQ(params.size(), 3);
    EXPECT_EQ(params.at("username"), "john");
    EXPECT_EQ(params.at("age"), "30");
    EXPECT_EQ(params.at("active"), "true");

    context.addBodyParam("age", "35");
    EXPECT_EQ(context.getBodyParams().at("age"), "35");

    bool removed = context.removeBodyParam("username");
    EXPECT_TRUE(removed);
    EXPECT_EQ(context.getBodyParams().size(), 2);

    removed = context.removeBodyParam("NonExistent");
    EXPECT_FALSE(removed);

    context.clearBodyParams();
    EXPECT_TRUE(context.getBodyParams().empty());
}

TEST(ContextTest, TimeoutSettings) {
    Context context;

    context.setConnectionTimeout(10);
    EXPECT_EQ(context.getConnectionTimeout(), 10);

    context.setReadTimeout(20);
    EXPECT_EQ(context.getReadTimeout(), 20);
}

TEST(ContextTest, ExitFlag) {
    Context context;

    EXPECT_FALSE(context.shouldExit());

    context.setShouldExit(true);
    EXPECT_TRUE(context.shouldExit());

    context.setShouldExit(false);
    EXPECT_FALSE(context.shouldExit());
}

}