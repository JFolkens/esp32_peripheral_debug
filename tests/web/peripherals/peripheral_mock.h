#include <gtest/gtest.h>

#include "main/web/peripherals/peripheral_interface.h"

namespace rover::tests::web
{

class PeripheralMock : public rover::web::PeripheralInterface
{
   public:
    explicit PeripheralMock(std::string name_, rover::web::PeripheralType mode_);
    virtual ~PeripheralMock() = default;

    std::string html_state() const override;

    std::string html_control() const override;

    void handle_update(const rover::hal::Parameters &parameters) override;
    rover::web::PeripheralType state_update_mode() const override;

    std::string value = "initial";
    rover::web::PeripheralType mode;
};
}  // namespace rover::tests::web