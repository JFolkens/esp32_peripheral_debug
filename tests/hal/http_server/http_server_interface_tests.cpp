#include <gtest/gtest.h>

#include <utility>
#include <vector>

#include "main/hal/http_server/http_server_interface.h"

namespace rover::tests::hal
{

TEST(HttpServerInterfaceTest, MissingRouteReturnsNotFound)
{
    rover::hal::HttpServerInterface server;
    const rover::hal::Response response =
        server.handle_request("/missing", "", rover::hal::HttpMethod::GET);

    EXPECT_EQ(response.status_code, 404);
    EXPECT_EQ(response.content_type, "text/plain");
    EXPECT_EQ(response.body, "Not Found");
}

TEST(HttpServerInterfaceTest, DispatchesMatchingRouteAndPreservesRequest)
{
    rover::hal::HttpServerInterface server;
    bool called = false;
    const auto submit_callback = [&called](const rover::hal::Request &request) {
        called = true;
        EXPECT_EQ(request.path, "/submit");
        EXPECT_EQ(request.parameters.at("value"), "payload");
        EXPECT_EQ(request.method, rover::hal::HttpMethod::POST);

        rover::hal::Response response;
        response.status_code = 201;
        response.content_type = "application/json";
        response.body = "created";
        return response;
    };
    server.add_endpoint("/submit", rover::hal::HttpMethod::POST, submit_callback);

    const rover::hal::Response response =
        server.handle_request("/submit", "value=payload", rover::hal::HttpMethod::POST);

    EXPECT_TRUE(called);
    EXPECT_EQ(response.status_code, 201);
    EXPECT_EQ(response.content_type, "application/json");
    EXPECT_EQ(response.body, "created");
}

TEST(HttpServerInterfaceTest, DispatchesRouteWithQueryString)
{
    rover::hal::HttpServerInterface server;
    bool called = false;
    const auto update_callback = [&called](const rover::hal::Request &request) {
        called = true;
        EXPECT_EQ(request.path, "/m_speed/update");
        EXPECT_EQ(request.parameters.at("value"), "47");

        rover::hal::Response response;
        response.body = "updated";
        return response;
    };
    server.add_endpoint("/m_speed/update", rover::hal::HttpMethod::GET, update_callback);

    const rover::hal::Response response =
        server.handle_request("/m_speed/update?value=47", "", rover::hal::HttpMethod::GET);

    EXPECT_TRUE(called);
    EXPECT_EQ(response.body, "updated");
}

TEST(HttpServerInterfaceTest, ParsesQueryAndFormParameters)
{
    rover::hal::HttpServerInterface server;
    const auto update_callback = [](const rover::hal::Request &request) {
        EXPECT_EQ(request.parameters.at("mode"), "forward");
        EXPECT_EQ(request.parameters.at("speed"), "50");
        EXPECT_EQ(request.parameters.at("label"), "front_left");
        return rover::hal::Response{};
    };
    server.add_endpoint("/update", rover::hal::HttpMethod::POST, update_callback);

    server.handle_request("/update?mode=forward&speed=25", "speed=50&label=front_left",
                          rover::hal::HttpMethod::POST);
}

TEST(HttpServerInterfaceTest, URIAndMethodMustMatchExactly)
{
    rover::hal::HttpServerInterface server;
    int get_calls = 0;
    int post_calls = 0;
    const auto get_callback = [&get_calls](const rover::hal::Request &) {
        get_calls++;
        return rover::hal::Response{};
    };
    const auto post_callback = [&post_calls](const rover::hal::Request &) {
        post_calls++;
        return rover::hal::Response{};
    };
    server.add_endpoint("/status", rover::hal::HttpMethod::GET, get_callback);
    server.add_endpoint("/status", rover::hal::HttpMethod::POST, post_callback);

    EXPECT_EQ(server.handle_request("/status", "", rover::hal::HttpMethod::GET).status_code, 200);
    EXPECT_EQ(server.handle_request("/status", "", rover::hal::HttpMethod::POST).status_code, 200);
    EXPECT_EQ(server.handle_request("/status/", "", rover::hal::HttpMethod::GET).status_code, 404);
    EXPECT_EQ(get_calls, 1);
    EXPECT_EQ(post_calls, 1);
}

TEST(HttpServerInterfaceTest, DuplicateRouteReplacesPreviousCallback)
{
    rover::hal::HttpServerInterface server;
    int old_calls = 0;
    int new_calls = 0;
    const auto old_callback = [&old_calls](const rover::hal::Request &) {
        old_calls++;
        return rover::hal::Response{};
    };
    const auto new_callback = [&new_calls](const rover::hal::Request &) {
        new_calls++;
        rover::hal::Response response;
        response.body = "new";
        return response;
    };
    server.add_endpoint("/replace", rover::hal::HttpMethod::GET, old_callback);
    server.add_endpoint("/replace", rover::hal::HttpMethod::GET, new_callback);

    const rover::hal::Response response =
        server.handle_request("/replace", "", rover::hal::HttpMethod::GET);

    EXPECT_EQ(old_calls, 0);
    EXPECT_EQ(new_calls, 1);
    EXPECT_EQ(response.body, "new");
}

}  // namespace rover::tests::hal