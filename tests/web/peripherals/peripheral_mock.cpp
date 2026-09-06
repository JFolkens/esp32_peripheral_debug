#include "peripheral_mock.h"

#include <utility>

namespace rover::tests::web
{

PeripheralMock::PeripheralMock(std::string name_, rover::web::PeripheralType mode_)
    : PeripheralInterface(std::move(name_)), mode(mode_)
{
    // Empty
}

std::string PeripheralMock::html_state() const
{
    return "value: " + value;
}

std::string PeripheralMock::html_control() const
{
    return "control";
}

void PeripheralMock::handle_update(const std::map<std::string, std::string> &parameters)
{
    value = parameters.at("value");
}

rover::web::PeripheralType PeripheralMock::state_update_mode() const
{
    return mode;
}

}  // namespace rover::tests::web