#pragma once

namespace rover::hal
{

enum class GpioDirection {
    INPUT,
    OUTPUT
};

class GpioInterface
{
   public:
    explicit GpioInterface(const GpioDirection &direction_);
    virtual ~GpioInterface() = default;

    void set(bool val);
    bool get() const;

    const GpioDirection direction;

   protected:
    virtual void _set(bool val) = 0;
    virtual bool _get() const = 0;
    bool state = false;
};

}  // namespace rover::hal