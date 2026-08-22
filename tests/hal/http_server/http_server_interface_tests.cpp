#include <gtest/gtest.h>

#include <utility>
#include <vector>

#include "main/hal/http_server/http_server_interface.h"

namespace rover::tests::hal
{

class HttpServerMock : public rover::hal::HttpServerInterface
{
   public:
    void mark_connected()
    {
        connected();
    }

    void mark_disconnected()
    {
        disconnected();
    }

    std::vector<std::pair<std::string, rover::hal::HttpMethod>> registrations;

   private:
    void register_endpoint(const std::string &uri,
                           rover::hal::HttpMethod method) override
    {
        registrations.emplace_back(uri, method);
    }
};

TEST(HttpServerInterfaceTest, MissingRouteReturnsNotFound)
{
    rover::hal::HttpServerInterface server;
    const rover::hal::Request request{"/missing", "",
                                      rover::hal::HttpMethod::GET};

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
                            EXPECT_EQ(request.method,
                                      rover::hal::HttpMethod::POST);

                            rover::hal::Response response;
                            response.status_code = 201;
                            response.content_type = "application/json";
                            response.body = "created";
                            return response;
                        });

    const rover::hal::Response response = server.handle_request(
        {"/submit", "payload", rover::hal::HttpMethod::POST});

    EXPECT_TRUE(called);
    EXPECT_EQ(response.status_code, 201);
    EXPECT_EQ(response.content_type, "application/json");
    EXPECT_EQ(response.body, "created");
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

    EXPECT_EQ(
        server.handle_request({"/status", "", rover::hal::HttpMethod::GET})
            .status_code,
        200);
    EXPECT_EQ(
        server.handle_request({"/status", "", rover::hal::HttpMethod::POST})
            .status_code,
        200);
    EXPECT_EQ(
        server.handle_request({"/status/", "", rover::hal::HttpMethod::GET})
            .status_code,
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

TEST(HttpServerInterfaceTest, ReadyTransportRegistersExistingRoutes)
{
    HttpServerMock server;
    server.add_endpoint("/one", rover::hal::HttpMethod::GET,
                        [](const rover::hal::Request &) {
                            return rover::hal::Response{};
                        });
    server.add_endpoint("/two", rover::hal::HttpMethod::POST,
                        [](const rover::hal::Request &) {
                            return rover::hal::Response{};
                        });

    server.mark_connected();

    ASSERT_EQ(server.registrations.size(), 2);
    EXPECT_EQ(server.registrations[0],
              std::make_pair(std::string("/one"), rover::hal::HttpMethod::GET));
    EXPECT_EQ(
        server.registrations[1],
        std::make_pair(std::string("/two"), rover::hal::HttpMethod::POST));
}

TEST(HttpServerInterfaceTest, NewRouteRegistersWhenTransportIsReady)
{
    HttpServerMock server;
    server.mark_connected();

    server.add_endpoint("/late", rover::hal::HttpMethod::PUT,
                        [](const rover::hal::Request &) {
                            return rover::hal::Response{};
                        });

    ASSERT_EQ(server.registrations.size(), 1);
    EXPECT_EQ(
        server.registrations[0],
        std::make_pair(std::string("/late"), rover::hal::HttpMethod::PUT));
}

TEST(HttpServerInterfaceTest, ReplacingReadyRouteDoesNotRegisterDuplicate)
{
    HttpServerMock server;
    server.add_endpoint("/replace", rover::hal::HttpMethod::DELETE,
                        [](const rover::hal::Request &) {
                            return rover::hal::Response{};
                        });
    server.mark_connected();

    server.add_endpoint("/replace", rover::hal::HttpMethod::DELETE,
                        [](const rover::hal::Request &) {
                            return rover::hal::Response{};
                        });

    ASSERT_EQ(server.registrations.size(), 1);
    EXPECT_EQ(server.registrations[0],
              std::make_pair(std::string("/replace"),
                             rover::hal::HttpMethod::DELETE));
}

TEST(HttpServerInterfaceTest, UnavailableTransportStopsLateRegistration)
{
    HttpServerMock server;
    server.mark_connected();
    server.mark_disconnected();

    server.add_endpoint("/offline", rover::hal::HttpMethod::GET,
                        [](const rover::hal::Request &) {
                            return rover::hal::Response{};
                        });

    EXPECT_TRUE(server.registrations.empty());
}

TEST(HttpServerInterfaceTest, ReadyTransportReRegistersRoutesAfterReconnect)
{
    HttpServerMock server;
    server.add_endpoint("/persistent", rover::hal::HttpMethod::GET,
                        [](const rover::hal::Request &) {
                            return rover::hal::Response{};
                        });

    server.mark_connected();
    server.mark_disconnected();
    server.mark_connected();

    ASSERT_EQ(server.registrations.size(), 2);
    EXPECT_EQ(server.registrations[0],
              std::make_pair(std::string("/persistent"),
                             rover::hal::HttpMethod::GET));
    EXPECT_EQ(server.registrations[1],
              std::make_pair(std::string("/persistent"),
                             rover::hal::HttpMethod::GET));
}

}  // namespace rover::tests::hal