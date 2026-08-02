#include "peripheral_interface.h"

namespace rover::web
{

PeripheralInterface::PeripheralInterface(std::string name_)
    : name(std::move(name_))
{
    // Empty
}

std::string PeripheralInterface::id() const
{
    return name;
}

}  // namespace rover::web
