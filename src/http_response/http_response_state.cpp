#include "http_response_state.hpp"
#include <iostream>
#include <fstream>

StatusCode check_file_error(const HttpRequest &request);
std::vector<unsigned char> read_file(const HttpRequest request);
StatusCode check_method_error(const HttpRequest &request);
std::string parse_type(const HttpRequest &request);

HttpResponseState::HttpResponseState() {}

//HttpResponseState::HttpResponseState(const HttpResponseState &src) {}

//HttpResponseState& HttpResponseState::operator=(const HttpResponseState &src) {}

HttpResponseState::~HttpResponseState() {}

void HttpResponseState::set_body(const HttpRequest& request) {

	std::vector<unsigned char> buffer;

	buffer = read_file(request);

	// data() allows us to print the body's content,
	// by returning a pointer to the first char of the vector
	body_.assign(buffer.begin(), buffer.end());
	std::cout << body_.data() << std::endl; // TODO: .data() is dangerous if buffer is not null terminated. overload print instead.
}

void HttpResponseState::set_statusCode(const HttpRequest& request) {
	StatusCode status;

	status = check_method_error(request);
	if (status == OK)
		status = check_file_error(request);
	std::cout << status << std::endl;
	statusCode_ = status;
}

StatusCode HttpResponseState::get_statusCode() {
	return statusCode_;
}


void HttpResponseState::set_headers(const HttpRequest& request) {
	// store content length
	add_header("Content-Length", std::to_string(body_.size()));
	// store content type

	std::string type = parse_type(request);
	add_header("Content-Type", type);
}

void HttpResponseState::add_header(const std::string& name, const std::string& value) {
	headers_.push_back({name, value});
}

// void HttpResponseState::read_headers() {
// 	for (const std::pair<std::string, std::string>& header : headers_) {
//     std::cout << header.first << ": " << header.second << std::endl;
// }
// }