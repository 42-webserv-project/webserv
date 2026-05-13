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
		const std::string httpVersion_ = "HTTP/1.1";
		StatusCode statusCode_;
		std::map<std::string, std::string> headers_; // TODO: change data structure, as it would lose duplicates
		std::vector<unsigned char> body_;

	public:
		HttpResponseState();
		HttpResponseState(const HttpResponseState &src);
		HttpResponseState& operator=(const HttpResponseState &src);
		~HttpResponseState();

		void set_body(std::vector <unsigned char> line);
};

#endif 