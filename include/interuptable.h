#pragma once

#include <atomic>
#include <thread>

class Interuptable {
public:
    Interuptable() noexcept = default;

    Interuptable(Interuptable &) = delete;

    Interuptable & operator=(Interuptable &) = delete;

    virtual ~Interuptable() noexcept;

    void stop() noexcept;

    void start();

    void join();

    bool isWorking() const noexcept;

    bool isStopped() const noexcept;

protected:
    // The step() method is called every time within loop() method.
    // There is no need to loop inside the step method.
    virtual void step() noexcept = 0;

    virtual void onStop() noexcept;

    virtual void onThreadStart() noexcept;

    virtual void onThreadFinish() noexcept;

private:
    // Runs in a new thread
    void loop() noexcept;

private:
    std::atomic_bool mWorking;
    std::thread mThread;
};
