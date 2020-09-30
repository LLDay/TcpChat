# TcpChat
Server receives messages from clients and broadcasts them back.

# Build
Requirements: Linux, Qt

```bash
git clone https://github.com/LLDay/TcpChat.git
cd TcpChat
mkdir build && cd build
cmake .. && cmake --build . -j4
```

Server and client programs will be placed in `../TcpClient/build/bin/`

To close the server send SIGINT to it (using \<Ctrl-C\> or kill utility)

# Server parts
Server combines **ConnectionListener**, **IncomingEventsListener** and **WorkerPool**. It uses from 3 to (2 + \<Workers Number\>) threads.
Optimal number of workers may equals `std::thread::hardware_concurency()`.

**ManualControl** interface provides methods for manually starting and stopping object's work, as well as checking its state. Many classes implement `onStart()` and `onStop()` event methods and `join()` method.

**LoopedThread** runs a function in a loop within new thread. Provides `onThreadStart()` and `onThreadFinish()` event methods and one pure virtual `threadStep()` method.

**ConnectionListener** accepts client's connections. It calls `onNewConnection()` method of the server.

**IncomingEventsListener** reacts on incoming messages and closed connections. It calls `onIncomingMessageFrom(int)` and `onConnectionLost(int)` methods of the server.

**WorkersPool** runs several thread and wakes them up when a new task adds to the tasks queue.
Server uses Tasks to parallel send and receive operations.

# Client parts

Client uses adapter of **IncomingEventsListener** that implements Qt's signal and slot mechanism to receive messages.

There is no need to use the thread pool for i/o operations.

On incoming message signal, client shows widget with the message.
