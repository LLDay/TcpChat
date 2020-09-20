#include "io_operations.h"

#include "server_log.h"

#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>

IoReadTask::IoReadTask(int socket, CallbackType callback) noexcept
    : mSocket{socket}, mCallback{callback} {}

void IoReadTask::run() noexcept {
    std::array<char, BUFFER_SIZE> buffer;
    std::vector<char> rawMessage;

    auto bytes = read(mSocket, buffer.begin(), buffer.size());

    if (bytes < 0) {
        serverError("read");
        return;
    }

    do {
        std::copy(
            buffer.begin(), buffer.begin() + bytes,
            std::back_inserter(rawMessage));

        bytes = read(mSocket, buffer.begin(), buffer.size());
    } while (bytes > 0);

    serverInfo("Received " + std::to_string(rawMessage.size()) + " bytes");

    if (mCallback) {
        auto readMessage =
            Message::deserialize(rawMessage.data(), rawMessage.size());
        readMessage.datetime = time(nullptr);
        mCallback(readMessage);
    }
}

IoBroadcastTask::IoBroadcastTask(
    const SocketList & sockets,
    const Message & message) noexcept
    : mSockets{std::make_shared<SocketList>(sockets)},
      mMessage{std::make_shared<Message>(message)} {}

void IoBroadcastTask::run() noexcept {
    auto serialized = mMessage->serialize();

    for (int i = mFrom; i < mTo; ++i) {
        if (write((*mSockets)[i], serialized.data(), serialized.size()) < 0)
            serverError("write");
    }
}

IoBroadcastTask::BroadcastTaskList IoBroadcastTask::split(int splits) noexcept {
    const auto size = mTo - mFrom;
    const auto splitsNumber = std::min(splits, size);
    const auto part = size / splitsNumber;

    BroadcastTaskList tasks;
    for (auto i = 1; i < splitsNumber; ++i) {
        IoBroadcastTask copy{*this};
        copy.mFrom = this->mFrom + (i - 1) * part;
        copy.mTo = i * part;
        tasks.emplace_back(std::make_unique<IoBroadcastTask>(std::move(copy)));
    }

    mFrom = (splitsNumber - 1) * part;
    tasks.emplace_back(std::make_unique<IoBroadcastTask>(std::move(*this)));
    return tasks;
}
