#ifndef HTTP_RESPONSE_STATE_H
 # define HTTP_RESPONSE_STATE_H

#include <string>
#include <vector>
#include <map>
#include <cstddef>
#include "config-mock.hpp"

enum StatusCode
{
	OK = 200,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	OTHER = -1,
};

class HttpResponseState {
	private:
		const std::string httpVersion_ = "HTTP/1.1";
		StatusCode statusCode_;
		std::vector<std::pair<std::string, std::string>> headers_;
		std::vector<unsigned char> body_;

	public:
		HttpResponseState();
		//HttpResponseState(const HttpResponseState &src);
		//HttpResponseState& operator=(const HttpResponseState &src);
		~HttpResponseState();

		void set_body(const HttpRequest& request);
		void set_statusCode(const HttpRequest& request);
		StatusCode get_statusCode();
		void set_headers(const HttpRequest& request);
		void add_header(const std::string& name, const std::string& value);
		std::string serialize (void) ;
		// //temp debug
		// void read_headers();
};

#endif 
