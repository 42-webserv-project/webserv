#include "logger/logger.hpp"
#include "http_response/http_response_state.hpp"
#include "http_response/config-mock.hpp"

int read_file(const HttpRequest request);

int main()
{
	HttpRequest request;
	PrintMsg("Hello user!");
	read_file(request);
	return 0;
}