#include "device_registry.h"

// Forward declaration - implemented in devices_config.cpp
void load_devices(rover::devices::HardwareRegistry& registry);

namespace rover::devices
{

HardwareRegistry::HardwareRegistry()
{
    // Load all devices from configuration
    load_devices(*this);
}

HardwareRegistry& HardwareRegistry::instance()
{
    static HardwareRegistry registry;
    return registry;
}

const std::vector<DeviceDescriptor>& HardwareRegistry::get_all() const
{
    return devices;
}

size_t HardwareRegistry::device_count() const
{
    return devices.size();
}

void HardwareRegistry::add_device(const DeviceDescriptor& descriptor)
{
    devices.push_back(descriptor);
}

}  // namespace rover::devices
