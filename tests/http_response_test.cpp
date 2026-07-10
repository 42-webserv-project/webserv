#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <filesystem>
#include <fstream>
#include "../src/http_response/http_response_state.hpp"

static HttpRequest make_request(HttpMethod method, const std::string &path)
{
	HttpRequest request;

	request.method_ = method;
	request.path_ = path;
	return request;
}

static std::string status_text(StatusCode status)
{
	if (status == OK)
		return "OK";
	if (status == FORBIDDEN)
		return "FORBIDDEN";
	if (status == NOT_FOUND)
		return "NOT FOUND";
	if (status == NOT_ALLOWED)
		return "METHOD NOT ALLOWED";
	return "CODE_READ_ERR";
}

static std::string expected_response(StatusCode status, const std::string &content_type, const std::string &body = "")
{
	std::string response;

	response = "HTTP/1.1 ";
	response += std::to_string(status);
	response += " ";
	response += status_text(status);
	response += "\r\n";
	response += "Content-Length: ";
	response += std::to_string(body.size());
	response += "\r\n";
	response += "Content-Type: ";
	response += content_type;
	response += "\r\n\r\n";
	response += body;
	return response;
}

struct TempFile
{
	std::filesystem::path path_;

	TempFile(const std::filesystem::path &path, const std::string &content)
		: path_(path)
	{
		std::ofstream output(path_.c_str(), std::ios::binary | std::ios::trunc);
		output << content;
	}

	~TempFile()
	{
		std::error_code ec;
		std::filesystem::remove(path_, ec);
	}
};

static void check_response(const HttpRequest &request, StatusCode expected_status, const std::string &content_type, const std::string &body = "")
{
	HttpResponseState response;

	CHECK(response.fill_response(request) == 0);
	CHECK(response.get_statusCode() == expected_status);
	CHECK(response.serialize() == expected_response(expected_status, content_type, body));
}

TEST_CASE("Simple Check Get Request Method")
{
	check_response(make_request(Get, "tests/mock_files/simple.html"), OK, "text/html");
}

TEST_CASE("Readable file body is serialized")
{
	const TempFile file("tests/build/http_response_body.txt", "Hello, world!");

	check_response(make_request(Get, file.path_.string()), OK, "text/plain", "Hello, world!");
}

TEST_CASE("Non-GET requests are rejected")
{
	check_response(make_request(Post, "tests/mock_files/simple.html"), NOT_ALLOWED, "text/html");
}

TEST_CASE("Missing files map content types from extension")
{
	const struct Case
	{
		const char *path;
		const char *type;
	} cases[] = {
		{"tests/missing.html", "text/html"},
		{"tests/missing.css", "text/css"},
		{"tests/missing.json", "application/json"},
		{"tests/missing.png", "image/png"},
		{"tests/missing.jpg", "image/jpeg"},
		{"tests/missing.txt", "text/plain"},
		{"tests/missing.md", "application/octet-stream"},
	};

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
		check_response(make_request(Get, cases[i].path), NOT_FOUND, cases[i].type);
}

TEST_CASE("Empty paths are not found")
{
	check_response(make_request(Get, ""), NOT_FOUND, "");
}

TEST_CASE("Directory paths are forbidden and have no content type")
{
	HttpResponseState response;
	HttpRequest request = make_request(Get, "tests/mocks/directory_test");

	CHECK(response.fill_response(request) == 0);
	CHECK(response.get_statusCode() == FORBIDDEN);
	CHECK(response.serialize().find("HTTP/1.1 403 FORBIDDEN\r\n") == 0);
	CHECK(response.serialize().find("Content-Type: \r\n") != std::string::npos);
}

/*TEST_CASE("Unreadable files are forbidden")
{
	check_response(make_request(Get, "tests/mocks/wrong_permission_file.html"), FORBIDDEN, "text/html");
}*/