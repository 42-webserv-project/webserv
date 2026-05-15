#include "http_response_state.hpp"
#include "config-mock.hpp"
#include <iostream>
#include <fstream>

// read file: 
	// check if file exists, 
	// determine content type
	// get file size to set length
// generate response object
// send it over to socket

// read file into memory
int read_file(const HttpRequest request) {
	HttpResponseState response;
	std::string line;

	// open file in binary mode
	std::ifstream input(request.path_, std::ifstream::binary);
	if (input) {
		// check size of the file
		input.seekg(0, input.end);
		int size = input.tellg();
		input.seekg(0, input.beg);

		// allocate memory
		char *buffer = new char[size];

		// read data as a block
		input.read(buffer, size);

		input.close();

		// write content to buffer
		std::cout.write(buffer, size);

		//std::cout << buffer << std::endl;

		delete[] buffer;
	}
	return 0;
}