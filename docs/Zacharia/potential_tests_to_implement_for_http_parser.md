// TODO: Can you have a POST request without a body? (or an empty one?)
TEST_CASE("POST Request with empty body (Content-Length: 0)") {
	HttpParser p;

	p.Feed("POST /api/endpoint HTTP/1.1\r\n");
	p.Feed("Host: api.example.com\r\n");
	p.Feed("Content-Length: 0\r\n");
	p.Feed("\r\n");

	CHECK(p.GetMethod() == Post);
	CHECK(p.GetPath() == "/api/endpoint");
	CHECK(p.GetVersion() == "HTTP/1.1");
	CHECK(p.GetBodyExpectedLength() == 0);
	CHECK(p.GetBody() == "");
	CHECK(p.GetExternalState() == Complete);
	CHECK(p.GetInternalState() == Done);
}

TEST_CASE("Invalid HTTP Version") {
	HttpParser p;

	p.Feed("GET / HTTP/2.0\r\n");
	// The parser will be stuck waiting for more data since the version parsing fails silently
	// It will return NeedMoreData rather than immediately flagging InvalidRequest
	CHECK(p.GetExternalState() == NeedMoreData);
}

TEST_CASE("Missing colon in header") {
	HttpParser p;

	p.Feed("GET / HTTP/1.1\r\n");
	p.Feed("Host example.com\r\n");
	// The parser waits for the header end marker (\r\n\r\n) before processing
	// This will result in the parser waiting for more data
	CHECK(p.GetExternalState() == NeedMoreData);
}

TEST_CASE("Header with whitespace before colon") {
	HttpParser p;

	p.Feed("GET / HTTP/1.1\r\n");
	p.Feed("Host : example.com\r\n");
	p.Feed("\r\n");

	CHECK(p.GetMethod() == Get);
	auto headers = p.GetHeaders();
	CHECK(headers.find("host") != headers.end());
	CHECK(headers["host"] == "example.com");
	CHECK(p.GetExternalState() == Complete);
}

< TEST_CASE("GET request with complex path and query string") {
< 	HttpParser p;
< 
< 	p.Feed("GET /path/to/resource?query=value&other=123 HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("\r\n");
< 
< 	CHECK(p.GetMethod() == Get);
< 	CHECK(p.GetPath() == "/path/to/resource?query=value&other=123");
< 	CHECK(p.GetVersion() == "HTTP/1.1");
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< TEST_CASE("Request line with multiple spaces between method and path") {
< 	HttpParser p;
< 
< 	p.Feed("GET  / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("\r\n");
< 
< 	// This might be treated as path with a space or could fail
< 	// depending on implementation strictness
< 	CHECK(p.GetMethod() == Get);
< }
< 
< TEST_CASE("POST request with exact body size matching Content-Length") {
< 	HttpParser p;
< 
< 	std::string body = "Hello, World!";
< 	
< 	p.Feed("POST / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("Content-Length: 13\r\n");
< 	p.Feed("\r\n");
< 	p.Feed(body.c_str());
< 
< 	CHECK(p.GetBody() == "Hello, World!");
< 	CHECK(p.GetBodyExpectedLength() == 13);
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< TEST_CASE("Header field names are case-insensitive") {
< 	HttpParser p;
< 
< 	p.Feed("GET / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("CONTENT-TYPE: text/plain\r\n");
< 	p.Feed("Connection: close\r\n");
< 	p.Feed("\r\n");
< 
< 	auto headers = p.GetHeaders();
< 	CHECK(headers.find("host") != headers.end());
< 	CHECK(headers.find("content-type") != headers.end());
< 	CHECK(headers.find("connection") != headers.end());
< 	CHECK(headers["content-type"] == "text/plain");
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< TEST_CASE("GET request with root path") {
< 	HttpParser p;
< 
< 	p.Feed("GET / HTTP/1.1\r\n");
< 	p.Feed("Host: localhost:8080\r\n");
< 	p.Feed("\r\n");
< 
< 	CHECK(p.GetPath() == "/");
< 	CHECK(p.GetMethod() == Get);
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< TEST_CASE("Empty header value") {
< 	HttpParser p;
< 
< 	p.Feed("GET / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("X-Custom-Header: \r\n");
< 	p.Feed("\r\n");
< 
< 	auto headers = p.GetHeaders();
< 	CHECK(headers.find("x-custom-header") != headers.end());
< 	CHECK(headers["x-custom-header"] == "");
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< // ============================================================================
< // BUG HUNTING TEST CASES
< // ============================================================================
< 
< TEST_CASE("BUG: Body size larger than Content-Length should fail") {
< 	// BUG: The parser checks if buffer size > body_expected_len and marks as Error
< 	// But the test data has EXACT size, let's test with extra data
< 	HttpParser p;
< 
< 	std::string body = "Hello, World!";
< 	
< 	p.Feed("POST / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("Content-Length: 13\r\n");
< 	p.Feed("\r\n");
< 	p.Feed(body.c_str());
< 	p.Feed("EXTRA_DATA");  // This extra data should cause error
< 
< 	CHECK(p.GetExternalState() == InvalidRequest);
< 	CHECK(p.GetInternalState() == Error);
< }
< 
< TEST_CASE("BUG: Path with only whitespace should be rejected") {
< 	// TODO comment at line 379: "What if input is: GET  HTTP/1.0"
< 	// The current implementation treats the space as part of the path
< 	HttpParser p;
< 
< 	p.Feed("GET  HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("\r\n");
< 
< 	// This should either fail or handle it gracefully
< 	// Current behavior: path_ = " " (single space)
< 	CHECK(p.GetPath() == "");  // Should be empty, not a space
< }
< 
< TEST_CASE("BUG: Assignment operator doesn't copy member variables") {
< 	// Lines 47-54: The assignment operator is empty (does nothing)
< 	HttpParser p1;
< 	p1.Feed("GET /test HTTP/1.1\r\n");
< 	p1.Feed("Host: example.com\r\n");
< 	p1.Feed("\r\n");
< 
< 	HttpParser p2;
< 	p2 = p1;  // Assignment operator
< 
< 	// These should be equal but they won't be due to the empty operator=
< 	CHECK(p2.GetMethod() == Get);
< 	CHECK(p2.GetPath() == "/test");
< 	CHECK(p2.GetVersion() == "HTTP/1.1");
< }
< 
< TEST_CASE("BUG: ParseMethod failure doesn't set Error state") {
< 	// Lines 364-365: If ParseMethod returns -1, state is not set to Error
< 	// This causes the parser to continue in RequestLine state
< 	HttpParser p;
< 
< 	p.Feed("INVALID_METHOD / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("\r\n");
< 
< 	// Should be in Error state, but it's likely NeedMoreData
< 	CHECK(p.GetInternalState() == Error);
< 	CHECK(p.GetExternalState() == InvalidRequest);
< }
< 
< TEST_CASE("BUG: Empty method string parsing") {
< 	// Edge case: what if method is empty?
< 	HttpParser p;
< 
< 	p.Feed(" / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("\r\n");
< 
< 	CHECK(p.GetExternalState() == InvalidRequest);
< }
< 
< TEST_CASE("BUG: Content-Length with leading zeros") {
< 	// Should be treated as valid decimal (e.g., "00013" = 13)
< 	HttpParser p;
< 
< 	std::string body = "Hello, World!";
< 	
< 	p.Feed("POST / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("Content-Length: 00013\r\n");
< 	p.Feed("\r\n");
< 	p.Feed(body.c_str());
< 
< 	CHECK(p.GetBodyExpectedLength() == 13);
< 	CHECK(p.GetBody() == "Hello, World!");
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< TEST_CASE("BUG: Reusing parser after completion (parser state not reset)") {
< 	// Bug: Parser doesn't have a reset mechanism
< 	// Feeding new data after completion might cause issues
< 	HttpParser p;
< 
< 	// First request
< 	p.Feed("GET /first HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("\r\n");
< 
< 	CHECK(p.GetExternalState() == Complete);
< 	CHECK(p.GetPath() == "/first");
< 
< 	// Try feeding second request (parser should be reusable)
< 	p.Feed("GET /second HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("\r\n");
< 
< 	// Second request should be parsed, but parser is in Done state
< 	CHECK(p.GetPath() == "/second");
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< TEST_CASE("BUG: Multiple colons in header value") {
< 	// Edge case: URL-like header value with colons
< 	HttpParser p;
< 
< 	p.Feed("GET / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
< 	p.Feed("X-URL: http://example.com:8080\r\n");
< 	p.Feed("\r\n");
< 
< 	auto headers = p.GetHeaders();
< 	CHECK(headers.find("x-url") != headers.end());
< 	CHECK(headers["x-url"] == "http://example.com:8080");
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< TEST_CASE("BUG: Header with tab as OWS") {
< 	// Test optional whitespace (tabs vs spaces)
< 	HttpParser p;
< 
< 	p.Feed("GET / HTTP/1.1\r\n");
< 	p.Feed("Host:\texample.com\r\n");  // Tab instead of space
< 	p.Feed("\r\n");
< 
< 	auto headers = p.GetHeaders();
< 	CHECK(headers.find("host") != headers.end());
< 	CHECK(headers["host"] == "example.com");
< 	CHECK(p.GetExternalState() == Complete);
< }
< 
< TEST_CASE("BUG: Zero-length body with extra trailing data") {
< 	// Content-Length is 0 but there's data after headers
< 	HttpParser p;
< 
< 	p.Feed("POST / HTTP/1.1\r\n");
< 	p.Feed("Host: example.com\r\n");
	p.Feed("Content-Length: 0\r\n");
	p.Feed("\r\n");
	p.Feed("UNEXPECTED_DATA");

	// Should succeed with zero-length body and ignore trailing data
	CHECK(p.GetBodyExpectedLength() == 0);
	CHECK(p.GetBody() == "");
	CHECK(p.GetExternalState() == Complete);
}

TEST_CASE("BUG: Incomplete body (partial data)") {
	// If body is incomplete, should stay in NeedMoreData
	HttpParser p;

	p.Feed("POST / HTTP/1.1\r\n");
	p.Feed("Host: example.com\r\n");
	p.Feed("Content-Length: 13\r\n");
	p.Feed("\r\n");
	p.Feed("Hello");  // Only 5 bytes of 13

	CHECK(p.GetExternalState() == NeedMoreData);
	CHECK(p.GetInternalState() == Body);
}

TEST_CASE("BUG: Both Transfer-Encoding and Content-Length present") {
	// According to RFC, this should be rejected (already implemented)
	// But let's verify it works correctly
	HttpParser p;

	p.Feed("POST / HTTP/1.1\r\n");
	p.Feed("Host: example.com\r\n");
	p.Feed("Transfer-Encoding: chunked\r\n");
	p.Feed("Content-Length: 5\r\n");
	p.Feed("\r\n");

	CHECK(p.GetExternalState() == InvalidRequest);
	CHECK(p.GetInternalState() == Error);
}

TEST_CASE("BUG: Version parsed but method failed - state consistency") {
	// What happens if version is valid but method is invalid?
	HttpParser p;

	p.Feed("BADMETHOD / HTTP/1.1\r\n");

	// Method parsing fails, but version string is valid
	// Does the parser properly set Error state?
	CHECK(p.GetExternalState() == InvalidRequest);
	CHECK(p.GetInternalState() == Error);
}

TEST_CASE("BUG: Very large Content-Length value (near size_t max)") {
	// Edge case with overflow protection
	HttpParser p;

	p.Feed("POST / HTTP/1.1\r\n");
	p.Feed("Host: example.com\r\n");
	p.Feed("Content-Length: 99999999999999999999\r\n");  // Very large
	p.Feed("\r\n");

	// ParseDecimalSize should handle overflow
	CHECK(p.GetExternalState() == InvalidRequest);
}
