# World Cup Community Updates

## Overview

This project implements a **community-led World Cup update subscription service**, allowing users to subscribe to game channels and exchange live updates.

- **Server**: Java, supports **Thread-Per-Client (TPC)** and **Reactor** models.
- **Client**: C++, interacts with the server using **STOMP 1.2 protocol**.

Users can:
- Subscribe/unsubscribe to game channels.
- Report game events from JSON files.
- Receive live updates from other users.
- Summarize game events into a file.
- Login/logout gracefully.

---

## Server (Java)

### Key Interfaces

- **Connections<T>**
  - `send(int connectionId, T msg)`
  - `send(String channel, T msg)`
  - `disconnect(int connectionId)`

- **ConnectionHandler<T>**
  - `send(T msg)`

- **StompMessagingProtocol**
  - `start(int connectionId, Connections<String> connections)`
  - `process(String message)`
  - `shouldTerminate()`

### Server Modes

- **Thread-Per-Client (TPC)**
- **Reactor**

### Run Commands

```bash
# Build
mvn compile

# Thread-per-client server
mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="<port> tpc"

# Reactor server
mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="<port> reactor"
```

### STOMP Protocol Frames

#### Client → Server
- CONNECT
- SEND
- SUBSCRIBE
- UNSUBSCRIBE
- DISCONNECT

#### Server → Client
- CONNECTED
- MESSAGE
- RECEIPT
- ERROR

## Client (C++)

### Executable
`bin/StompWCIClient`

### Threads
- **Keyboard thread**: Reads user commands and sends STOMP frames.  
- **Socket thread**: Listens to incoming frames from the server.  

### Commands
- `login {host:port} {username} {password}`  
- `join {game_name}`  
- `exit {game_name}`  
- `report {file.json}` — Sends game events to server  
- `summary {game_name} {user} {file}` — Outputs game summary  
- `logout`  

### Error Handling
- Login errors (`wrong password`, `already logged in`)  
- Frame errors from server  
- Receipt acknowledgements for join/exit/logout  

### Game Events
Game event JSON contains:
- `event name`  
- `time` (in seconds)  
- `general game updates` (global stats)  
- `team a updates`, `team b updates`  
- `description`  

**Parser provided:** `event.h` and `event.cpp` with  
`parseEventsFile(std::string json_path)`  

### Run Commands
1. Navigate to the `Client` directory:  
   ```bash
   cd Client
   ```
2. Build the client using the provided Makefile:
    ```bash
   make
   ```
3. Run the client executable:
   ```bash
   ./bin/StompWCIClient
   ```

