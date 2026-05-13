#include "http_response_state.hpp"
#include "config-mock.cpp"
#include <iostream>
#include <fstream>

// read file: 
	// check if file exists, 
	// determine content type
	// get file size to set length
// generate response object
// send it over to socket

int read_file(const HttpRequest request) {
	HttpResponseState response;
	std::string line;

	std::ifstream input(request.path_, std::ios_base::in);
	while (std::getline(input, line)) {
		
	}
}