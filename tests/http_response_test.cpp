#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/http_response/http_response_state.hpp"

#include "../src/http_request/http_request.hpp"

TEST_CASE("Simple Check Get Request Method")
{
	HttpRequest r;

	r.method_ = Post;

	HttpResponseState response;
	response.fill_response(r);
	CHECK(response.get_statusCode() == OK);
}
// Test cases to build:
/* 
	- get request
	- later : post request/delete/other?
	- missing request
	- lowercase request
	- directory request
	- later: cgi?
	- existing path
	- missing path


*/