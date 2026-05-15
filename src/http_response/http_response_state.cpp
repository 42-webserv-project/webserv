#include "http_response_state.hpp"
#include <iostream>
#include <fstream>

HttpResponseState::HttpResponseState() {}

//HttpResponseState::HttpResponseState(const HttpResponseState &src) {}

//HttpResponseState& HttpResponseState::operator=(const HttpResponseState &src) {}

HttpResponseState::~HttpResponseState() {}

void HttpResponseState::set_body(std::vector<unsigned char> buffer) {
	
	// data() allows us to print the body's content, 
	//by returning a pointer to the first char of the vector
	body_.assign(buffer.begin(), buffer.end());
	std::cout << body_.data() << std::endl;
}

