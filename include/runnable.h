#pragma once

class IRunnable {
public:
    virtual ~IRunnable() noexcept = default;

    virtual void run() noexcept = 0;
};
