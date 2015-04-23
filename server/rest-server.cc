// clang++ -g -o server server.cc -I/opt/local/include -L/opt/local/lib /opt/local/lib/libcppnetlib-server-parsers.a /opt/local/lib/libcppnetlib-uri.a -lssl -lcrypto -lboost_system-mt 
#include <boost/network/protocol/http/server.hpp>
#include <string>
#include <iostream>

namespace http = boost::network::http;

struct hello_world;
typedef http::server<hello_world> server;

struct hello_world {
  /*<< This is the function that handles the incoming request. >>*/
  void operator()(server::request const &request, server::response &response) {
    server::string_type ip = source(request);
    unsigned int port = request.source_port;
    std::ostringstream data;
    data << "Hello, " << ip << ':' << port << '!';
    response = server::response::stock_reply(server::response::ok, data.str());
  }
  /*<< It's necessary to define a log function, but it's ignored in
       this example. >>*/
  void log(...) {
    // do nothing
  }
};
int
main(int argc, char * argv[]) 
{
    hello_world handler;
    server::options options(handler);
    options.address("127.0.0.1")
           .port("8080");
    try {
        server server_(options);
        server_.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
