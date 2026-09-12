#include <gtest/gtest.h>

#include <memory>

#include "main/web/device_web_app.h"
#include "peripherals/peripheral_mock.h"

namespace rover::tests::web
{

TEST(DeviceWebAppTest, RegistersStateAndUpdateRoutes)
{
    auto server = std::make_unique<rover::hal::HttpServerInterface>();
    rover::hal::HttpServerInterface *server_ptr = server.get();
    rover::web::DeviceWebApp app(std::move(server));

    auto peripheral = std::make_unique<PeripheralMock>("test", rover::web::PeripheralType::Sensor);
    PeripheralMock *peripheral_ptr = peripheral.get();
    app.add_peripheral(std::move(peripheral));

    const rover::hal::Response state =
        server_ptr->handle_request("/test/state", "", rover::hal::HttpMethod::GET);
    EXPECT_EQ(state.status_code, 200);
    EXPECT_EQ(state.content_type, "text/html");
    EXPECT_EQ(state.body, "value: initial");

    const rover::hal::Response update =
        server_ptr->handle_request("/test/update", "value=updated", rover::hal::HttpMethod::POST);
    EXPECT_EQ(update.status_code, 200);
    EXPECT_EQ(update.content_type, "text/html");
    EXPECT_EQ(update.body, "value: updated");
    EXPECT_EQ(peripheral_ptr->value, "updated");

    const rover::hal::Response page =
        server_ptr->handle_request("/", "", rover::hal::HttpMethod::GET);
    EXPECT_NE(page.body.find("id=\"test_state\""), std::string::npos);
    EXPECT_NE(page.body.find("/test/state"), std::string::npos);
    EXPECT_EQ(page.body.find("{\"ok\":true}"), std::string::npos);
}

}  // namespace rover::tests::web