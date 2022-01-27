# RemoteCounter

This process is a server that handles up to 1024 connections simultaneously and maintains a 
single counter which each connection can increment or decrement.

## Code Structure

- Main file handles all socket connections and polls on them using epoll
- Server class creates the listener socket for incoming connections
- Counter class keeps track of count
- Log class is a singleton class used for logging to syslog

## Build

```sh
mkdir build
cd build
cmake ..
make
```

## Systemd Service

This process can easily be integrated with systemd by creating a unit file for the service. Below
are key lines that must be included.

```sh
# Process should start after syslog is running
After=syslog.service

# Restart if fail with a 5s holdoff
Restart=on-failure
RestartSec=5s

# Send stderr to journal
StandardError=journal
```
