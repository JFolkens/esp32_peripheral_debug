#pragma once

namespace rover::hal
{

class MotorInterface
{
   public:
    virtual ~MotorInterface() = default;

    virtual void set(int speed) = 0;
    virtual int get() const = 0;
};

}  // namespace rover::hal