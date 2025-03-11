#pragma once

#include <httplib.h>
#include <json/json.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <mutex>
#include <condition_variable>

namespace lunarica {
namespace testing {

class TestHttpServer {
public:
    TestHttpServer(int port = 8080) : port_(port), running_(false) {}

    ~TestHttpServer() {
        stop();
    }

    void start() {
        if (running_) return;

        running_ = true;
        server_thread_ = std::thread([this]() {
            server_.Get("/posts/1", [](const httplib::Request&, httplib::Response& res) {
                Json::Value response;
                response["id"] = 1;
                response["title"] = "Test Post";
                response["body"] = "This is a test post";
                response["userId"] = 1;

                Json::FastWriter writer;
                std::string json = writer.write(response);

                res.set_header("Content-Type", "application/json");
                res.set_content(json, "application/json");
            });

            server_.Post("/posts", [](const httplib::Request& req, httplib::Response& res) {
                Json::Value requestBody;
                Json::CharReaderBuilder builder;
                std::string errors;
                std::istringstream requestStream(req.body);
                Json::parseFromStream(builder, requestStream, &requestBody, &errors);

                Json::Value response = requestBody;
                response["id"] = 101;

                Json::FastWriter writer;
                std::string json = writer.write(response);

                res.status = 201;
                res.set_header("Content-Type", "application/json");
                res.set_content(json, "application/json");
            });

            server_.Put("/posts/1", [](const httplib::Request& req, httplib::Response& res) {
                Json::Value requestBody;
                Json::CharReaderBuilder builder;
                std::string errors;
                std::istringstream requestStream(req.body);
                Json::parseFromStream(builder, requestStream, &requestBody, &errors);

                Json::Value response = requestBody;
                response["id"] = 1;

                Json::FastWriter writer;
                std::string json = writer.write(response);

                res.set_header("Content-Type", "application/json");
                res.set_content(json, "application/json");
            });

            server_.Delete("/posts/1", [](const httplib::Request&, httplib::Response& res) {
                res.set_header("Content-Type", "application/json");
                res.set_content("{}", "application/json");
            });

            server_.Get("/query", [](const httplib::Request& req, httplib::Response& res) {
                Json::Value response;

                for (const auto& [key, values] : req.params) {
                    response["args"][key] = values;
                }

                Json::FastWriter writer;
                std::string json = writer.write(response);

                res.set_header("Content-Type", "application/json");
                res.set_content(json, "application/json");
            });

            server_.Get("/headers", [](const httplib::Request& req, httplib::Response& res) {
                Json::Value response;

                for (const auto& [key, value] : req.headers) {
                    response["headers"][key] = value;
                }

                Json::FastWriter writer;
                std::string json = writer.write(response);

                res.set_header("Content-Type", "application/json");
                res.set_content(json, "application/json");
            });

            server_.Post("/echo", [](const httplib::Request& req, httplib::Response& res) {
                res.set_header("Content-Type", req.get_header_value("Content-Type"));
                res.set_content(req.body, req.get_header_value("Content-Type"));
            });

            server_.Get("/error", [](const httplib::Request&, httplib::Response& res) {
                res.status = 500;
                res.set_header("Content-Type", "application/json");
                res.set_content("{\"error\": \"Internal Server Error\"}", "application/json");
            });

            server_.Get("/timeout", [](const httplib::Request&, httplib::Response&) {
                std::this_thread::sleep_for(std::chrono::seconds(10));
            });

            server_.Get("/not-found", [](const httplib::Request&, httplib::Response& res) {
                res.status = 404;
                res.set_header("Content-Type", "application/json");
                res.set_content("{\"error\": \"Not Found\"}", "application/json");
            });

            server_.listen("localhost", port_);
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void stop() {
        if (!running_) return;

        server_.stop();
        if (server_thread_.joinable()) {
            server_thread_.join();
        }
        running_ = false;
    }

    int getPort() const {
        return port_;
    }

    std::string getBaseUrl() const {
        return "http://localhost:" + std::to_string(port_);
    }

private:
    httplib::Server server_;
    int port_;
    std::thread server_thread_;
    std::atomic_bool running_;
};

}
}