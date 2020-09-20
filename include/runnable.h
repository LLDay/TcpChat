#pragma once

#include <functional>
#include <tuple>

class IRunnable {
public:
    virtual ~IRunnable() noexcept = default;

    virtual void run() noexcept = 0;
};
