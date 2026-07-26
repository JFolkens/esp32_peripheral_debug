#pragma once

namespace rover::hal
{

class LedInterface
{
   public:
    virtual ~LedInterface() = default;

    virtual void set(bool val) = 0;
    virtual bool get() const = 0;

    void toggle();
};

}  // namespace rover::hal