#include "led_interface.h"

namespace rover::hal
{

void LedInterface::toggle()
{
    set(~get());
}

}  // namespace rover::hal