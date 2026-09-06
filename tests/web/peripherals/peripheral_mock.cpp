#include "peripheral_mock.h"

namespace rover::tests::web
{

PeripheralMock::PeripheralMock(std::string name_, rover::web::PeripheralType mode_)
    : PeripheralInterface(name), mode(mode_)
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

void PeripheralMock::handle_update(const rover::hal::Parameters &parameters)
{
    value = parameters.at("value");
}

rover::web::PeripheralType PeripheralMock::state_update_mode() const
{
    return mode;
}

}  // namespace rover::tests::web