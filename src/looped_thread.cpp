#include "looped_thread.h"

LoopedThread::~LoopedThread() noexcept {
    stop();
    join();
}

void LoopedThread::join() noexcept {
    if (mThread.joinable())
        mThread.join();
}

void LoopedThread::onThreadStart() noexcept {}

void LoopedThread::onThreadFinish() noexcept {}

void LoopedThread::loop() noexcept {
    onThreadStart();

    while (isWorking())
        threadStep();

    onThreadFinish();
}

void LoopedThread::onStart() noexcept {
    mThread = std::thread{&LoopedThread::loop, this};
}
