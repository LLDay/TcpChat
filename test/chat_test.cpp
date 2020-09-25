#include "connection_listener.h"
#include "io_operations.h"
#include "message.h"
#include "task.h"
#include "utils.h"
#include "workers_pool.h"

#include <gtest/gtest.h>
#include <unistd.h>
#include <mutex>

Message getMessage() {
    Message message;
    message.author = "Author";
    message.text = "Text\nWith new line";
    message.datetime = time(nullptr);
    return message;
}

TEST(Server, Message) {
    auto message = getMessage();

    auto serialized = message.serialize();
    auto deserialized =
        Message::deserialize(serialized.data(), serialized.size());

    ASSERT_EQ(message.author, deserialized.author);
    ASSERT_EQ(message.text, deserialized.text);
    ASSERT_EQ(message.datetime, deserialized.datetime);
}

class CounterTask : public ITask {
public:
    void run() noexcept override {
        increment();
    }

    static void increment() noexcept {
        std::lock_guard lock{sMutex};
        sCounter += 1;
    }

    static int result() noexcept {
        std::lock_guard lock{sMutex};
        return sCounter;
    }

    static void reset() noexcept {
        std::lock_guard lock{sMutex};
        sCounter = 0;
    }

private:
    static int sCounter;
    static std::mutex sMutex;
};

int CounterTask::sCounter = 0;
std::mutex CounterTask::sMutex;

TEST(Server, Workers) {
    auto workersNumber = 4;
    auto tasksNumber = 1000;
    auto loop = 100;

    for (auto i = 0; i < loop; ++i) {
        CounterTask::reset();

        WorkersPool pool{workersNumber};
        pool.start();

        for (auto j = 0; j < tasksNumber; ++j)
            pool.addTask(std::make_unique<CounterTask>());

        pool.stop();
        pool.join();

        ASSERT_EQ(CounterTask::result(), tasksNumber);
    }
}

TEST(Server, ReadTask) {
    int pipes[2];
    ASSERT_EQ(pipe(pipes), 0);

    makeNonBlocking(pipes[0]);

    auto message = getMessage();
    auto serialized = message.serialize();

    auto callback = [&message](const Message & recvMessage) {
        ASSERT_EQ(message.author, recvMessage.author);
        ASSERT_EQ(message.text, recvMessage.text);
        ASSERT_EQ(message.datetime, recvMessage.datetime);
    };

    write(pipes[1], serialized.data(), serialized.size());
    IoReadTask readTask(pipes[0], callback);
    readTask.run();

    close(pipes[0]);
    close(pipes[1]);
}

TEST(Server, BroadcastTask) {
    auto messagesNumber = 300;
    auto message = getMessage();
    std::vector<int> fdRead;
    std::vector<int> fdWrite;
    CounterTask::reset();

    int pipes[2];
    for (auto i = 0; i < messagesNumber; ++i) {
        ASSERT_EQ(pipe(pipes), 0);
        makeNonBlocking(pipes[0]);
        fdRead.push_back(pipes[0]);
        fdWrite.push_back(pipes[1]);
    }

    WorkersPool poolWrite{4};
    IoBroadcastTask broadcastTask{fdWrite, message};

    poolWrite.start();
    for (auto & task : broadcastTask.split(11))
        poolWrite.addTask(std::move(task));

    auto callback = [&message](const Message & recvMessage) {
        ASSERT_EQ(message.author, recvMessage.author);
        ASSERT_EQ(message.text, recvMessage.text);
        ASSERT_EQ(message.datetime, recvMessage.datetime);
        CounterTask::increment();
    };
    poolWrite.stop();
    poolWrite.join();

    WorkersPool poolRead{4};
    poolRead.start();

    for (auto read : fdRead) {
        auto readTask = std::make_unique<IoReadTask>(read, callback);
        poolRead.addTask(std::move(readTask));
    }

    poolRead.stop();
    poolRead.join();

    ASSERT_EQ(CounterTask::result(), messagesNumber);

    for (auto fd : fdRead)
        close(fd);

    for (auto fd : fdWrite)
        close(fd);
}
