# webserv
Building our own HTTP server from scratch.

## Current Work Distribution

To maintain a clear workflow, the core areas of responsibility are distributed as follows:

### Tetiana
- **Infrastructure & Standards:** Project scaffolding, build system (`Makefile`), and development conventions.
- **Core Engine:** Event loop architecture and connection handling.
- **CGI Module:** Integration and execution of CGI scripts.

### Zach
- **Request Management:** Handling the incoming HTTP request lifecycle.
- **Parser Implementation:** Developing the core algorithm for HTTP request parsing and validation.

### Jo
- **Response Generation:** Processing server logic, routing, and formatting the final HTTP response (headers and payload) to be transmitted by the event loop.
- **Configuration Module:** Parsing, validating, and applying server configuration files.

## Build

**make**              — build project  
**make DBG=1**        — build with debug logging  
**make DBG=1 LOG=1**  — build with debug logging and log file  
**make test**         — run all tests in tests/  
**make re**           — full rebuild  
**make clean**        — remove object files  
**make fclean**       — remove object files and binary  