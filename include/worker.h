#pragma once

#include "looped_thread.h"

class WorkersPoolAccessor;

class Worker : public LoopedThread {
public:
    explicit Worker(WorkersPoolAccessor & pool) noexcept;

    ~Worker() noexcept = default;

private:
    void threadStep() noexcept override;

private:
    WorkersPoolAccessor & mPool;
};
