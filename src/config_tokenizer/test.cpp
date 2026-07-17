#include "config_tokenizer.hpp"
#include <iostream>

int main()
{
	std::string src = ""
"server {"
"	listen 127.0.0.1:8080;"
"	listen 127.0.0.2:8081;"
"	listen 8082; # defaults host to 0.0.0.0 (nginx behavior), making the server accessible from any IP address on the machine"
"	#listen 127.0.0.1; # no port = default to 80 (nginx behavior)"
"	#listen 127.0.0.1:4242; # trying a closed port or one already in use returns a bind error, but the server still starts and listens on the other valid ports, which is the expected behavior"
""
"	# Set up default error pages"
"	error_page 404 /errors/404.html;"
""
"	# Set the maximum allowed size for client request bodies."
"	client_max_body_size 10M;"
"	# Default root for the server, used if no root is specified in a location block"
"	root www;"
""
"	# Specify rules or configurations on a URL/route (no regex required here), for a website, among the following::";

	ConfigTokenizer ct(src);

	ConfigToken token = ct.Next();

	while (token.type != ConfigToken::EndOfFile)
	{
		std::cout << "Type: " << token.type << "\tValue: " << token.value << std::endl;
		token = ct.Next();
	}
	return (0);
}
