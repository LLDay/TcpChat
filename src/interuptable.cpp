#include <interuptable.h>

Interuptable::~Interuptable() noexcept {
    stop();
    if (mThread.joinable())
        mThread.join();
}

void Interuptable::start() {
    mThread = std::thread{&Interuptable::loop, this};
}

void Interuptable::join() {
    mThread.join();
}

bool Interuptable::isWorking() const noexcept {
    return mWorking;
}

bool Interuptable::isStopped() const noexcept {
    return !mWorking;
}

void Interuptable::stop() noexcept {
    mWorking = false;
    onStop();
}

void Interuptable::onStop() noexcept {}

void Interuptable::onThreadStart() noexcept {}

void Interuptable::onThreadFinish() noexcept {}

void Interuptable::loop() noexcept {
    onThreadStart();

    while (mWorking)
        step();

    onThreadFinish();
}
