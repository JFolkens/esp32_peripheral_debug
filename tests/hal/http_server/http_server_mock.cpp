#include "http_server_mock.h"

namespace rover::tests::hal
{
void HttpServerMock::mark_connected()
{
    connected();
}

void HttpServerMock::mark_disconnected()
{
    disconnected();
}

void HttpServerMock::register_endpoint(const std::string &uri,
                                       rover::hal::HttpMethod method,
                                       const rover::hal::endpoint &ep)
{
    registrations.emplace_back(uri, method);
}
}  // namespace rover::tests::hal