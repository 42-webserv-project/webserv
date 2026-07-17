#include "http_response.hpp"

StatusCode check_file_error(const HttpRequest &request);
std::vector<unsigned char> read_file(const HttpRequest request);
StatusCode check_method_error(const HttpRequest &request);
std::string parse_type(const HttpRequest &request);

HttpResponse::HttpResponse() : statusCode_(OTHER) {}

HttpResponse::HttpResponse(const HttpResponse &src)
    : statusCode_(src.statusCode_), headers_(src.headers_), body_(src.body_)
{
}

HttpResponse &HttpResponse::operator=(const HttpResponse &src)
{
	if (this != &src)
	{
		statusCode_ = src.statusCode_;
		headers_ = src.headers_;
		body_ = src.body_;
	}
	return (*this);
}

HttpResponse::~HttpResponse() {}

// Load the response body from the requested file.
void HttpResponse::set_body(const HttpRequest &request)
{
	std::vector<unsigned char> buffer;

	buffer = read_file(request);

	body_.assign(buffer.begin(), buffer.end());
}

// Set the response status from the request.
void HttpResponse::set_status_code(const HttpRequest &request)
{
	StatusCode status;

	status = check_method_error(request);
	if (status == OK)
		status = check_file_error(request);
	statusCode_ = status;
}

// Return the current response status.
StatusCode HttpResponse::get_status_code()
{
	return statusCode_;
}

// Add the basic response headers.
void HttpResponse::set_headers(const HttpRequest &request)
{
	add_header("Content-Length", std::to_string(body_.size()));

	std::string type = parse_type(request);
	add_header("Content-Type", type);
}

// Add one header to the response.
void HttpResponse::add_header(const std::string &name, const std::string &value)
{
	headers_.push_back({name, value});
}

// Turn a status code into its reason phrase.
static std::string convert_status_code(int code) {
	if (code == 200)
		return "OK";
	else if (code == 403)
		return "FORBIDDEN";
	else if (code == 404)
		return "NOT FOUND";
	else if (code == 405)
		return "METHOD NOT ALLOWED";	
	return "CODE_READ_ERR"; // change default to application/octet-stream or 500 internal server?
}

// Build the final HTTP response string.
std::string HttpResponse::serialize(void)
{
	std::string response;
	int code = static_cast<int>(this->statusCode_);
	std::string status_str = convert_status_code(code);

	response.append(httpVersion_);
	response.append(" ");
	response.append(std::to_string(code));
	response.append(" ");
	response.append(status_str);
	response.append("\r\n");

	for (const auto &h : headers_)
	{
		response.append(h.first);
		response.append(": ");
		response.append(h.second);
		response.append("\r\n");
	}

	response.append("\r\n");

	if (!body_.empty())
	{
		response.append(reinterpret_cast<const char *>(body_.data()), body_.size());
	}
	return response;
}

// Fill all response fields from the request.
int	HttpResponse::fill_response(const HttpRequest &request) 
{
	set_status_code(request);
	set_body(request);
	set_headers(request);
	serialize();

	return (0);
}