#pragma once

#include <memory>
#include <string>

#include "../../drivetrain/drivetrain_omni.h"
#include "peripheral_interface.h"

namespace rover::web
{

/**
 * @class DrivetrainOmniWeb
 * @brief Expose a DrivetrainOmni instance to the web interface.
 *
 * Handles HTML control and display of a DrivetrainOmni instance.
 */
class DrivetrainOmniWeb : public PeripheralInterface
{
   public:
    DrivetrainOmniWeb(const std::string &name_, std::unique_ptr<rover::DrivetrainOmni> drivetrain);
    virtual ~DrivetrainOmniWeb() = default;

    std::string html_state() const override;
    std::string html_control() const override;
    void handle_update(const std::map<std::string, std::string> &parameters) override;

   private:
    std::unique_ptr<rover::DrivetrainOmni> _drivetrain;
};

}  // namespace rover::web
