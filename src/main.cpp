#include "logger/logger.hpp"
#include "http_response/http_response_state.hpp"

int fill_response(const HttpRequest &request);

int main()
{
	HttpRequest request;
	PrintMsg("Hello user!");
	fill_response(request);
	
	return 0;
}