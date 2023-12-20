# server_client

C implementation of server/multi-client. The connection is established using Linux socket.

## Compilation

A simple `make` will suffice

## Client and server options

Server can accept the following options:
-p: Port number.
-n: Maximum number of clients.
-b: Maximum length to which the queue of pending connections may grow.
-h: Display help.

Client can accept the following options:
-p: Port number.
-a: IPv4 address of the server.
-h: Display help.
