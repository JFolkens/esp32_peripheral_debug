#pragma once

#include <functional>
#include <string>
#include <vector>

namespace rover::web
{
class PeripheralInterface;
}

namespace rover::devices
{

/**
 * @enum DeviceType
 * @brief Enumeration of supported device types
 */
enum class DeviceType {
    LED,      ///< GPIO-based LED
    MOTOR,    ///< Motor controller (L298N)
    PWM,      ///< PWM signal generator
};

/**
 * @struct DeviceDescriptor
 * @brief Metadata describing a single hardware device and its factory function
 *
 * Each descriptor encapsulates:
 * - Device identification (id, name)
 * - Device type classification
 * - Factory function that creates the hardware and its web wrapper
 */
struct DeviceDescriptor {
    /// Unique identifier for this device (e.g., "green_led", "motor_front_left")
    std::string id;

    /// Human-readable name for display in logs/UI
    std::string name;

    /// Type of device (LED, MOTOR, PWM)
    DeviceType type;

    /// Factory function that creates the hardware and wraps it in PeripheralInterface.
    /// Returns PeripheralInterface* (the factory owns all underlying hardware memory).
    std::function<rover::web::PeripheralInterface*()> factory;

    /**
     * @brief Construct a device descriptor
     * @param id_ Unique identifier
     * @param name_ Human-readable name
     * @param type_ Device type
     * @param factory_ Factory function returning PeripheralInterface*
     */
    DeviceDescriptor(const std::string& id_, const std::string& name_, DeviceType type_,
                     std::function<rover::web::PeripheralInterface*()> factory_)
        : id(id_), name(name_), type(type_), factory(factory_)
    {
    }
};

/**
 * @class HardwareRegistry
 * @brief Central registry for all hardware devices
 *
 * The registry maintains a static list of device descriptors and provides
 * iteration for initialization. Each descriptor contains a factory function
 * that creates the hardware and wraps it in PeripheralInterface.
 */
class HardwareRegistry
{
   public:
    /**
     * @brief Get the singleton instance of the registry
     * @return Reference to the hardware registry
     */
    static HardwareRegistry& instance();

    /**
     * @brief Get all device descriptors
     * @return Const reference to the vector of descriptors
     */
    const std::vector<DeviceDescriptor>& get_all() const;

    /**
     * @brief Get number of devices in the registry
     * @return Count of registered devices
     */
    size_t device_count() const;

    /**
     * @brief Add a device descriptor to the registry
     * @param descriptor The device descriptor to add
     */
    void add_device(const DeviceDescriptor& descriptor);

   private:
    /// Private constructor (singleton)
    HardwareRegistry();

    /// Static vector of device descriptors
    std::vector<DeviceDescriptor> devices;
};

}  // namespace rover::devices
