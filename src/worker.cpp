#include "worker.h"

#include "workers_pool.h"

#include <iostream>

Worker::Worker(WorkersPoolAccessor & pool) noexcept : mPool{pool} {}

void Worker::threadStep() noexcept {
    auto task = mPool.waitForTask();
    if (task != nullptr)
        task->run();
}
