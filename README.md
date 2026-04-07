# Chat-application-in-C
Peer-to-peer chat application in C using TCP sockets and POSIX threads
# tcp-chat-c

**Work in progress**: This project is actively being developed. New features and improvements are being added regularly.

A peer-to-peer terminal chat application written in **pure C**, using TCP sockets for real-time communication and POSIX threads (pthreads) for concurrent sending and receiving.

## How it works

Each instance of the program runs both a **client** and a **server** simultaneously:

- The **server thread** listens for an incoming connection from the other peer
- The **client thread** connects to the other peer's machine
- Once both connections are established, two additional threads handle **sending** and **receiving** messages in parallel

This allows both users to send and receive messages at the same time without blocking each other.

## Requirements
- Linux (Ubuntu, Debian, etc.)
- GCC compiler
- pthreads library (usually included with GCC on Linux, in libc)

## Build
  gcc -o chat main.c -lpthread

## Usage

Both machines must run the program. Each machine connects to the other's IP address.

**On machine A** — set `server_IP` to machine B's IP address, then run:
./chat

**On machine B** — set `server_IP` to machine A's IP address, then run:
./chat

Once connected, simply type your message and press **Enter** to send. Received messages appear automatically.

Press **Ctrl+D** to quit.

---

## Project structure

```
tcp-chat-c/
├── main.c        # Main source file
└── README.md     # This file
```

---

## Current features

- [x] TCP socket connection between two machines
- [x] Simultaneous send and receive using threads
- [x] Automatic reconnection if the server is not yet ready
- [x] Colored terminal output 

## Planned features

- [ ] Command-line arguments for IP address and port
- [ ] Username / display name
- [ ] Message timestamps
- [ ] File transfer support
- [ ] Graceful disconnection handling
- [ ] Support for more than two peers
- [ ] Windows compatibility (Win32 threads + Winsock)
- [ ] Encryption (TLS/SSL)

## Known limitations

- IP address is currently hardcoded in the source file
- Only supports one-to-one communication (two peers)
- No message history


## Contributing

This project is a work in progress. Feel free to open issues or submit pull requests — all contributions are welcome.
