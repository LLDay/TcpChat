#pragma once

#include <atomic>

class ManualControl {
public:
    virtual ~ManualControl() noexcept;

    void start() noexcept;

    void stop() noexcept;

    bool isWorking() const noexcept;

    bool isStopped() const noexcept;

protected:
    virtual void onStart() noexcept;

    virtual void onStop() noexcept;

private:
    std::atomic_bool mWorking = false;
};
