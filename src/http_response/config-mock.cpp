#include <string>
#include <map>

struct HttpRequest {
	std::string method_ = "GET";
	// Path relative to the project root used by tests
	std::string path_ = "test/response_test/mock_site/simple.html";
	std::string version_ = "HTTP/1.1";
	std::map<std::string, std::string> headers_ = {
		{"Host", "localhost:8080"},
		{"Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"},
		{"Connection", "close"},
		{"User-Agent", "test-client/1.0"}
	};
	std::string body_;
};