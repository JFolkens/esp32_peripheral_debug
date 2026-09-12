#include "device_web_app.h"

#include <sstream>

namespace rover::web
{

DeviceWebApp::DeviceWebApp(std::unique_ptr<rover::hal::HttpServerInterface> server_)
    : server(std::move(server_))
{
    // Endpoint for landing page, "/"
    const auto page_callback = [this](const rover::hal::Request &req) {
        rover::hal::Response response = {};
        response.body = render_page();
        return response;
    };
    server->add_endpoint("/", rover::hal::HttpMethod::GET, page_callback);
}

void DeviceWebApp::add_peripheral(std::unique_ptr<PeripheralInterface> peripheral)
{
    // Each peripheral can handle two functions (endpoints):
    // "/state", which reads hardware and updates html
    // "/update", which takes parameters and modifies the hardware
    PeripheralInterface *peripheral_ptr = peripheral.get();
    const auto state_cb = [peripheral_ptr](const rover::hal::Request &) {
        rover::hal::Response response;
        response.content_type = "text/html";
        response.body = peripheral_ptr->html_state();
        return response;
    };
    const std::string state_uri = "/" + peripheral->id() + "/state";
    server->add_endpoint(state_uri, rover::hal::HttpMethod::GET, state_cb);

    const auto update_cb = [peripheral_ptr](const rover::hal::Request &request) {
        rover::hal::Response response;
        response.content_type = "text/html";
        response.body = peripheral_ptr->update_and_render_state(request.parameters);
        return response;
    };
    const std::string update_uri = "/" + peripheral->id() + "/update";
    server->add_endpoint(update_uri, rover::hal::HttpMethod::POST, update_cb);

    // std::move must come after pointer access
    peripherals.push_back(std::move(peripheral));
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
        html += peripheral->html_state_and_control();
    }

    std::stringstream polling_script;
    polling_script << "        <script>\n"
                   << "        (() => {\n"
                   << "            const sensorStates = [\n";
    bool first_sensor = true;
    for (const auto &peripheral : peripherals) {
        if (peripheral->state_update_mode() != PeripheralType::Sensor)
            continue;
        if (!first_sensor)
            polling_script << ",\n";
        polling_script << "                { url: '/" << peripheral->id() << "/state', target: '"
                       << peripheral->id() << "_state' }";
        first_sensor = false;
    }
    polling_script << "\n            ];\n"
                   << "\n"
                   << "            async function pollSensorStates() {\n"
                   << "                for (const sensor of sensorStates) {\n"
                   << "                    const response = await fetch(sensor.url);\n"
                   << "                    if (response.ok) {\n"
                   << "                        document.getElementById(sensor.target).innerHTML =\n"
                   << "                            await response.text();\n"
                   << "                    }\n"
                   << "                }\n"
                   << "            }\n"
                   << "\n"
                   << "            if (sensorStates.length > 0) {\n"
                   << "                setInterval(pollSensorStates, 1000);\n"
                   << "            }\n"
                   << "        })();\n"
                   << "        </script>\n"
                   << "        </body>\n"
                   << "    </html>\n";
    html += polling_script.str();

    return html;
}

}  // namespace  rover::web
