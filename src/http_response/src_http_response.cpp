#include "http_response_state.hpp"

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

// if path does not exist -> NOT_FOUND
// if path exists but cannot be read → FORBIDDEN
// if path exists and is readable → OK

StatusCode check_method_error(const HttpRequest &request)
{
	if (request.method_ == "GET")
		return OK;
	return OTHER; // TODO update with appropriate error code
}

StatusCode check_file_error(const HttpRequest &request)
{
	if (std::filesystem::exists(request.path_) == false)
		return NOT_FOUND;
	std::ifstream input(request.path_, std::ifstream::binary);
	if (!input)
		return FORBIDDEN;
	return OK;
}

int fill_response(const HttpRequest &request)
{
	HttpResponseState response;

	response.set_statusCode(request);
	if (response.get_statusCode() == OK)
	{
		response.set_body(request);
		response.set_headers(request);
		response.set_headers(request);
		response.serialize();
	}
	else
		return (-1);
	return 0;
}

// read file into memory
std::vector<unsigned char> read_file(const HttpRequest request)
{

	// open file in binary mode
	std::ifstream input(request.path_, std::ifstream::binary);
	// check size of the file
	input.seekg(0, input.end);
	int size = input.tellg();
	input.seekg(0, input.beg);

	// allocate memory
	std::vector<unsigned char> buffer(size);

	// read data as a block
	input.read(reinterpret_cast<char *>(buffer.data()), size);
	input.close();

	return buffer;
}

// extract content type from path
// todo: add fall back in case of empty string (default MIME type)
std::string parse_type(const HttpRequest &request)
{
	std::filesystem::path p(request.path_);
	// extension() returns the substring starting at the last dot of the filename
	std::string str = p.extension().string();
	if (!str.empty() && str.front() == '.')
		str.erase(0, 1);
	// std::cout << "here" << str << std::endl;
	return str;
}