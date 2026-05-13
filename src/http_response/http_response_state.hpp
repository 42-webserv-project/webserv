#ifndef HTTP_RESPONSE_STATE_H
 # define HTTP_RESPONSE_STATE_H

#include <string>
#include <vector>
#include <map>
#include <cstddef>

enum StatusCode {
	OK = 200,
};

class HttpResponseState {
	private:
		const std::string httpVersion = "HTTP/1.1";
		StatusCode statusCode;
		std::map<std::string, std::string> headers; // TODO: change data structure, as it would lose duplicates
		std::vector<std::byte> body;

	public:
		HttpResponseState();
		HttpResponseState(const HttpResponseState &src);
		HttpResponseState& operator=(const HttpResponseState &src);
		~HttpResponseState();
};

#endif 