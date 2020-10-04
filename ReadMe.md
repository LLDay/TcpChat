# TcpChat
Server receives messages from clients and broadcasts them back.

# Build
Requirements: docker, docker-compose >= 2.0

```bash
git clone https://github.com/LLDay/TcpChat.git
cd TcpChat
docker-compose up
```

Server and client programs will be placed in `../TcpClient/build/bin/`

To close the server send SIGINT to it (using \<Ctrl-C\> or kill utility)

# Message format
Server and client use this message format to communicate each other:
```
   server time       author        separator         text
|--- 8 bytes ---|--- n bytes ---|--- 1 byte ---|--- m bytes ---|

Total: 9 + length(author) + length(text) bytes
```

Server time field is aware of endianness.
New line symbol `\n` is used as separator. It means that author cannot have this symbol in the name.
You should use host with `sizeof(char)` == 1 (the most hosts meet this requirement).
Converting a Message object fields to the communication format and back implemented in [message.cpp](src/message.cpp) file.

# Arguments
To specify globally ip address or port you should set CHAT\_IP or CHAT\_PORT environment variables.
Also you can set them explicitly passing as arguments. Arguments have higher priority than environment variables.
If an ip address or a port are not specified then the server/client will connect to the address `127.0.0.1:50000`.

For example
```bash
server # 127.0.0.1:50000
client # 127.0.0.1:50000

export CHAT_IP=12012
export CHAT_PORT='10.152.40.31'

server # 10.152.40.31:12012
client # 10.152.40.31:12012

server -i 10.203.11.10 -p 2324 # 10.203.0.1:2324
client -i 10.203.11.10 -p 2324 # 10.203.0.1:2324

server -w 32 # specify number of parallel workers
server --help # to see info
```

# Hardcoded parameters
Some parameters such as **IncomingEventsListener**'s `timeout` and `eventBufferSize` are defined in [server\_main.src](./src/server_main.cpp) and [client\_main.cpp](./src/client/client_main.cpp).

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
