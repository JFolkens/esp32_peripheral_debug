#pragma once

#include <gmock/gmock.h>

#include "main/hal/http_server/http_server_interface.h"

namespace rover::tests::hal
{

class HttpServerMock : public rover::hal::HttpServerInterface
{
   public:
    void mark_connected();
    void mark_disconnected();

    std::vector<std::pair<std::string, rover::hal::HttpMethod>> registrations;

   private:
    void register_endpoint(const std::string &uri,
                           rover::hal::HttpMethod method,
                           const rover::hal::endpoint &ep) override;
};

}  // namespace rover::tests::hal
