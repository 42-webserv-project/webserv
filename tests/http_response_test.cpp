#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/http_response/http_response.hpp"



TEST_CASE("Simple Check Get Request Method")
{
	HttpRequest r;

	r.method_ = Get;
	r.path_ = "tests/mock_files/simple.html";

	HttpResponse response;

	response.fill_response(r);
	CHECK(response.get_status_code() == OK);
}



// Test cases to build:
/* 
// file that is a directory
// file does not exist

// problem: Root will be different on each PC, how will we have a file there if we fetch from github?
// file from root (~/somefileinroot)
	- get request
	- later : post request/delete/other?
	- missing request
	- lowercase request
	- directory request
	- later: cgi?
	- existing path
	- missing path


*/