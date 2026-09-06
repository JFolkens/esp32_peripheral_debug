#include "device_web_app.h"

namespace rover::web
{

DeviceWebApp::DeviceWebApp(rover::hal::HttpServerInterface &server_) : server(server_)
{
    // Endpoint
    // for
    // landing
    // page,
    // "/"
    server.add_endpoint("/", rover::hal::HttpMethod::GET, [this](const rover::hal::Request &req) {
        rover::hal::Response response = {};
        response.body = render_page();
        return response;
    });
}

void DeviceWebApp::add_peripheral(PeripheralInterface *peripheral)
{
    peripherals.push_back(peripheral);

    for (const EndpointDefinition &endpoint : peripheral->endpoints()) {
        auto ep_callback = [this, peripheral](const rover::hal::Request &req) {
            peripheral->handle_action(req);  // Update hardware
            rover::hal::Response response = {};
            response.body = render_page();  // Hardware status as html webpage
            return response;
        };
        server.add_endpoint(endpoint.uri, endpoint.method, ep_callback);
    }
}

std::string DeviceWebApp::render_page() const
{
    std::string html;
    html +=
        R"raw(<!DOCTYPE html>
    <html>
        <head>
            <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
            <style>
                html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }
                body { margin: 20px; }
                .button { background-color: #4CAF50; border: none; color: white; padding: 16px 16px; text-decoration: none; font-size: 24px; margin: 2px; cursor: pointer; }
                .button-off { background-color: #555555; }
                .card { margin: 20px auto; padding: 10px; border: 1px solid #ddd; border-radius: 8px; max-width: 640px; }
            </style>
        </head>
        <body>
            <h1>ESP32 Web Server</h1>
        )raw";

    for (const auto &peripheral : peripherals) {
        html += peripheral->render_html();
    }

    html +=
        R"raw(
        </body>
    </html>
    )raw";

    return html;
}

}  // namespace
   // rover::web
