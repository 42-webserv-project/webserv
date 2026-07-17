#include "http_response.hpp"
#include <algorithm>

// Check whether the request method is supported.
StatusCode	check_method_error(const HttpRequest &request)
{
	if (request.method_ == Get)
		return (OK);
	return (NOT_ALLOWED);
}

// Check whether the requested file can be read.
StatusCode	check_file_error(const HttpRequest &request)
{
	if (request.path_.empty())
		return (NOT_FOUND);
	std::error_code ec;
	const std::filesystem::path p(request.path_);
	if (!std::filesystem::exists(p, ec))
		return (NOT_FOUND);
	if (ec)
		return (FORBIDDEN);
	if (!std::filesystem::is_regular_file(p, ec))
		return (FORBIDDEN);
	if (ec)
		return (FORBIDDEN);
	std::ifstream input(p.c_str(), std::ifstream::binary);
	if (!input.is_open())
		return (FORBIDDEN);
	return (OK);
}

// Read the requested file into memory.
std::vector<unsigned char> read_file(const HttpRequest request)
{
	std::ifstream input(request.path_, std::ifstream::binary);
	if (!input.is_open())
		return {};

	input.seekg(0, std::ios::end);
	std::streamsize size = input.tellg();
	if (size < 0)
		return {};

	input.seekg(0, std::ios::beg);
	std::vector<unsigned char> buffer(size);
	if (size > 0)
		input.read(reinterpret_cast<char *>(buffer.data()), size);
	input.close();
	return (buffer);
}

// Map a file extension to a MIME type.
std::string complete_mime_type(std::string extension) {
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if (extension == "html")
    	return "text/html";
	else if (extension == "css")
		return "text/css";
	else if (extension == "json")
		return "application/json";
	else if (extension == "png")
		return "image/png";
	else if (extension == "jpg" || "jpeg" )
		return "image/jpeg";
	else if (extension == "txt")
		return "text/plain";
	return "application/octet-stream";
}

// Get the MIME type from the request path.
std::string parse_type(const HttpRequest &request)
{
	std::filesystem::path p(request.path_);
	std::string str = p.extension().string();
	if (!str.empty() && str.front() == '.') {
		str.erase(0, 1);
		str = complete_mime_type(str);
	}
	return (str);
}