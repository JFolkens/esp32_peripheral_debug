#include <gtest/gtest.h>

#include <utility>
#include <vector>

#include "main/hal/http_server/http_server_interface.h"

namespace rover::tests::hal
{

TEST(HttpServerInterfaceTest, MissingRouteReturnsNotFound)
{
    rover::hal::HttpServerInterface server;
    const rover::hal::Request request{"/missing", "", rover::hal::HttpMethod::GET};

    const rover::hal::Response response = server.handle_request(request);

    EXPECT_EQ(response.status_code, 404);
    EXPECT_EQ(response.content_type, "text/plain");
    EXPECT_EQ(response.body, "Not Found");
}

TEST(HttpServerInterfaceTest, DispatchesMatchingRouteAndPreservesRequest)
{
    rover::hal::HttpServerInterface server;
    bool called = false;
    server.add_endpoint("/submit", rover::hal::HttpMethod::POST,
                        [&called](const rover::hal::Request &request) {
                            called = true;
                            EXPECT_EQ(request.uri, "/submit");
                            EXPECT_EQ(request.body, "payload");
                            EXPECT_EQ(request.method, rover::hal::HttpMethod::POST);

                            rover::hal::Response response;
                            response.status_code = 201;
                            response.content_type = "application/json";
                            response.body = "created";
                            return response;
                        });

    const rover::hal::Response response =
        server.handle_request({"/submit", "payload", rover::hal::HttpMethod::POST});

    EXPECT_TRUE(called);
    EXPECT_EQ(response.status_code, 201);
    EXPECT_EQ(response.content_type, "application/json");
    EXPECT_EQ(response.body, "created");
}

TEST(HttpServerInterfaceTest, DispatchesRouteWithQueryString)
{
    rover::hal::HttpServerInterface server;
    bool called = false;
    server.add_endpoint("/m_speed/update", rover::hal::HttpMethod::GET,
                        [&called](const rover::hal::Request &request) {
                            called = true;
                            EXPECT_EQ(request.uri, "/m_speed/update?value=47");
                            EXPECT_EQ(request.parameters.at("value"), "47");

                            rover::hal::Response response;
                            response.body = "updated";
                            return response;
                        });

    const rover::hal::Response response =
        server.handle_request({"/m_speed/update?value=47", "", rover::hal::HttpMethod::GET});

    EXPECT_TRUE(called);
    EXPECT_EQ(response.body, "updated");
}

TEST(HttpServerInterfaceTest, ParsesQueryAndFormParameters)
{
    rover::hal::HttpServerInterface server;
    server.add_endpoint("/update", rover::hal::HttpMethod::POST,
                        [](const rover::hal::Request &request) {
                            EXPECT_EQ(request.parameters.at("mode"), "forward");
                            EXPECT_EQ(request.parameters.at("speed"), "50");
                            EXPECT_EQ(request.parameters.at("label"), "front left");
                            return rover::hal::Response{};
                        });

    server.handle_request({"/update?mode=forward&speed=25", "speed=50&label=front+left",
                           rover::hal::HttpMethod::POST});
}

TEST(HttpServerInterfaceTest, URIAndMethodMustMatchExactly)
{
    rover::hal::HttpServerInterface server;
    int get_calls = 0;
    int post_calls = 0;
    server.add_endpoint("/status", rover::hal::HttpMethod::GET,
                        [&get_calls](const rover::hal::Request &) {
                            get_calls++;
                            return rover::hal::Response{};
                        });
    server.add_endpoint("/status", rover::hal::HttpMethod::POST,
                        [&post_calls](const rover::hal::Request &) {
                            post_calls++;
                            return rover::hal::Response{};
                        });

    EXPECT_EQ(server.handle_request({"/status", "", rover::hal::HttpMethod::GET}).status_code, 200);
    EXPECT_EQ(server.handle_request({"/status", "", rover::hal::HttpMethod::POST}).status_code,
              200);
    EXPECT_EQ(server.handle_request({"/status/", "", rover::hal::HttpMethod::GET}).status_code,
              404);
    EXPECT_EQ(get_calls, 1);
    EXPECT_EQ(post_calls, 1);
}

TEST(HttpServerInterfaceTest, DuplicateRouteReplacesPreviousCallback)
{
    rover::hal::HttpServerInterface server;
    int old_calls = 0;
    int new_calls = 0;
    server.add_endpoint("/replace", rover::hal::HttpMethod::GET,
                        [&old_calls](const rover::hal::Request &) {
                            old_calls++;
                            return rover::hal::Response{};
                        });
    server.add_endpoint("/replace", rover::hal::HttpMethod::GET,
                        [&new_calls](const rover::hal::Request &) {
                            new_calls++;
                            rover::hal::Response response;
                            response.body = "new";
                            return response;
                        });

    const rover::hal::Response response =
        server.handle_request({"/replace", "", rover::hal::HttpMethod::GET});

    EXPECT_EQ(old_calls, 0);
    EXPECT_EQ(new_calls, 1);
    EXPECT_EQ(response.body, "new");
}

}  // namespace rover::tests::hal