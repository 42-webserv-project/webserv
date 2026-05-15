#include "http_response_state.hpp"
#include "config-mock.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <iostream>

// read file: 
	// check if file exists, 
	// determine content type
	// get file size to set length
// generate response object
// send it over to socket

// Generate HTTP response headers — You need to add:

// Status line: HTTP/1.1 200 OK\r\n
// Content-Type header (based on file extension)
// Content-Length header (from buffer size)
// Blank line to separate headers from body: \r\n
// Determine content type — Based on the file extension:

// .html → text/html
// .css → text/css
// .js → application/javascript
// etc.
// Build complete HTTP response — Combine status line + headers + body

// Send over socket — As mentioned in your comments

int fill_response(const std::vector<unsigned char> buffer) {
	HttpResponseState response;

	response.set_body(buffer);
	//response.set_status();
	return 0;
}

// read file into memory
int read_file(const HttpRequest request) {


	// open file in binary mode
	std::ifstream input(request.path_, std::ifstream::binary);
	if (input) {
		// check size of the file
		input.seekg(0, input.end);
		int size = input.tellg();
		input.seekg(0, input.beg);

		// allocate memory
		std::vector<unsigned char> buffer(size);

		// read data as a block
		input.read(reinterpret_cast<char*>(buffer.data()), size);

		input.close();

		// write content to buffer
		fill_response(buffer);


	}
	return 0;
}