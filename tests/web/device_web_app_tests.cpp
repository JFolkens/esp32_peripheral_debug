#include <gtest/gtest.h>

#include "main/web/device_web_app.h"
#include "peripherals/peripheral_mock.h"

namespace rover::tests::web
{

TEST(DeviceWebAppTest, RegistersStateAndUpdateRoutes)
{
    rover::hal::HttpServerInterface server;
    rover::web::DeviceWebApp app(server);
    PeripheralMock peripheral("test", rover::web::PeripheralType::Sensor);
    app.add_peripheral(&peripheral);

    const rover::hal::Response state =
        server.handle_request("/test/state", "", rover::hal::HttpMethod::GET);
    EXPECT_EQ(state.status_code, 200);
    EXPECT_EQ(state.content_type, "text/html");
    EXPECT_EQ(state.body, "value: initial");

    const rover::hal::Response update =
        server.handle_request("/test/update", "value=updated", rover::hal::HttpMethod::POST);
    EXPECT_EQ(update.status_code, 200);
    EXPECT_EQ(update.content_type, "application/json");
    EXPECT_EQ(update.body, "{\"ok\":true}");
    EXPECT_EQ(peripheral.value, "updated");

    const rover::hal::Response page = server.handle_request("/", "", rover::hal::HttpMethod::GET);
    EXPECT_NE(page.body.find("id=\"test_state\""), std::string::npos);
    EXPECT_NE(page.body.find("/test/state"), std::string::npos);
    EXPECT_EQ(page.body.find("{\"ok\":true}"), std::string::npos);
}

}  // namespace rover::tests::web