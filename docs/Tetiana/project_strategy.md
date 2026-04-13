# Webserv — Module Breakdown

C++17 | 3 people | nginx-style HTTP/1.1 server

---

## Before we start — things to decide together

A few things that touch all three modules. We need to align on these before anyone writes actual code:

- **`poll()` only** — the subject wants one multiplexing call for all I/O. `epoll` is Linux-only, `kqueue` is macOS-only. `poll` works on both, so that's I what to use.
- **CGI and the event loop** — before Module 1 writes anything, we all need to understand how CGI fits into the loop. CGI spawns a child process and communicates through pipes — those pipes are file descriptors that have to live in the same `poll` watchlist as client sockets. If we design the loop without thinking about this, we'll have to rewrite it later. Let's study CGI mechanics together during the theory phase.
- **Virtual hosts** — out of scope per the subject, but allowed. We need to decide this *before* the event loop is written. Retrofitting virtual host support into connection handling is painful; it's easy if the structure anticipates it from the start.
- **Shared data structures** — config structs (`ServerConfig`, `LocationBlock`) and request/response objects need to be defined together before anyone starts their module. No surprises when we try to plug things together.
- **Error handling** — agree on how we propagate errors across module boundaries. Pick one approach and stick to it.

---

## Module 1 — Network Core & Event Loop

**What this module owns:** the socket layer, connection lifecycle, and the central `poll` loop that drives everything.

**What needs to be implemented:**
- Socket setup: `socket`, `bind`, `listen`, `accept`
- Non-blocking I/O via `poll` — we don't need `fcntl`/`O_NONBLOCK` for this. Note: the subject only permits `fcntl` on macOS anyway, so on Linux we just don't use it, and the server works fine without it
- The main `poll` loop — monitoring client sockets *and* CGI pipes in the same loop
- The main poll loop ([Reactor pattern](https://en.wikipedia.org/wiki/Reactor_pattern)) — acting as the central event dispatcher. It monitors client sockets and CGI pipes synchronously, and dispatches events to the appropriate non-blocking handlers.
- Per-client state tracking (each connection is a state machine)
- Timeouts: both for idle clients and for CGI child processes that don't respond

**Hard constraints from the subject:**
- The server must never block — one stuck client shouldn't freeze everyone else
- `poll()` checks read *and* write at the same time
- Every I/O operation goes through `poll()`, no exceptions
- CGI pipes are file descriptors too — they go into the watchlist from day one, even before CGI is implemented

**Why it's interesting:** we will have to build a proper state machine to handle connections, especially when CGI pipes are added to the event loop. This is the hardest, but most useful part of the module.

**C++17 that helps here:** `std::variant` for connection states, `std::optional` for things that might not exist yet.

**Dependencies:**
- Needs config structs from Module 3 (to know which ports/hosts to bind)
- Hands raw request data to Module 2; gets response data back

---

## Module 2 — HTTP (Request & Response)

**What this module owns:** parsing incoming HTTP/1.1 requests and building correct responses.

**What needs to be implemented:**
- Request line: method, URI, HTTP version
- Header parsing and validation
- Body handling: `Content-Length` and chunked transfer encoding
- `GET`, `POST`, `DELETE`
- Static file serving with correct MIME types
- Error pages (400, 403, 404, 405, 500, ...)
- File uploads (POST body → disk)
- Directory listing (autoindex)
- HTTP redirects (301/302)

**Hard constraints from the subject:**
- Must serve a fully static website
- File upload must work
- `GET`, `POST`, `DELETE` are the mandatory minimum

**Why it's interesting:** real HTTP is messier than the spec. Partial reads, malformed headers, chunked encoding edge cases — all of this shows up. Writing a parser that handles it cleanly is good practice.

**C++17 that helps here:** `std::string_view` for parsing headers without extra allocations. `std::filesystem` for path handling and MIME type detection.

**Dependencies:**
- Needs raw buffers from Module 1 and location config from Module 3
- Gives Module 1 response buffers; triggers CGI in Module 3 when needed

---

## Module 3 — Config Parser & CGI

This module has two separate sub-components with different delivery timelines.

> Config parser needs to be done **first** — Module 1 and 2 can't do anything without it. CGI implementation can come later, but its design has to be understood before Module 1 starts writing the event loop.

### Config Parser

**What needs to be implemented:**
- Read and validate a nginx-style `.conf` file
- Parse `server` and `location` blocks with all their directives:
  - `listen`, `server_name`
  - `root`, `index`, `autoindex`
  - `error_page`, `client_max_body_size`
  - `allowed_methods`, `return`, `upload_store`
  - `cgi` (file extension → executable path)
- Build the structs that all other modules consume
- Handle bad config gracefully — either use defaults or exit with a clear error

**C++17 that helps here:** `std::filesystem` for path validation and extension checks. Structured bindings for cleaner iteration.

### CGI

**What needs to be implemented:**
- Detect CGI requests by file extension (`.php`, `.py`, etc.)
- Set environment variables per RFC 3875: `REQUEST_METHOD`, `QUERY_STRING`, `CONTENT_LENGTH`, `PATH_INFO`, etc.
- `fork()` — this is the **only place in the project where `fork` is allowed**
- Wire up stdin/stdout with `dup2` + `pipe`
- Run the script with `execve`
- Read the output and turn it into an HTTP response
- Handle timeout — the child process must not be able to hang the server

**Hard constraints from the subject:**
- `fork` only for CGI, nothing else
- At least one CGI interpreter must work (php-cgi, Python, etc.)
- Both `GET` and `POST` must work with CGI

**Why it's interesting:** config parser is a real format parser with actual validation logic. CGI is the full Unix process lifecycle in one feature — `fork`, `exec`, `pipe`, `dup2` — and it explains how dynamic web content worked before any framework existed.

**Dependencies:**
- Config structs → Module 1 and Module 2 *(deliver this first)*
- Needs request data from Module 2 to set up CGI environment
- Returns CGI response body to Module 2

---

## How modules connect

```
Module 3 (Config)
      │
      ▼
Module 1 (Network) ◄──────────────────────────┐
      │                                        │
      │  raw buffers                responses  │
      ▼                                        │
Module 2 (HTTP) ──► Module 3 (CGI) ───────────┘
```

---

## Integration order

1. Together: define shared structs and interfaces, decide on virtual hosts, study CGI together
2. Module 3 delivers config structs
3. Module 1 builds the event loop — with CGI pipe slots already in the design, even if CGI isn't implemented yet
4. Module 2 implements HTTP parsing and responses
5. Module 3 implements CGI — plugs into the hooks Module 1 already prepared
6. Together: stress testing (`siege`, `ab`), edge cases, eval prep