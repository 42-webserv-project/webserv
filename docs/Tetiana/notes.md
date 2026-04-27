# Event Loop & Shared Structures

---

## Event Loop

engine of the server  
watches all open fds (sockets, clients, pipes) and dispatches I/O without blocking

### poll() vs epoll()

**poll()**
- portable, works on macOS + Linux natively
- O(n) scan on every call — gets slower with many connections
- fine for our scale (a few hundred clients tops)

**epoll()** (Linux only)
- O(1) event delivery — kernel tracks ready fds
- doesn't exist on macOS natively
- to test on MacBook → need Docker (Ubuntu container)

---

## Loop structure

### start
- Read config file (ports, routes) - Johanna
- Setup listening sockets

### main loop (while true)

### wait for events
- check sockets for read/write readiness (poll/epoll)

### accept
- if listening socket is ready, accept new client connection
- add new client to the list of active sockets

### read and parse
- read data from client
- check if the full HTTP request is received
- parse method, path, and headers – Zacharia

### processing
- match the requested path to a route in the config (who will do router part? Jo?)
- get the requested content (read static file, run CGI, or prepare error page) – Jo
- build the HTTP response (status code, headers, body) – Zacharia

### send
- if client socket is ready to write, send the response
- keep track of what was sent (in case it sends in chunks)

### cleanup (RAII ?)
- if keep-alive, reset state for the next request
- if done or error, close connection and remove from active list
- check for and drop timed-out connections

---

### still need to check

- what struct represents a client?
- how does the handler signal "response ready" - enum
- timeout policy
- max connections limit?
- later on: CGI pipes — managed by me or by CGI module? asynchronous reading ?

---
