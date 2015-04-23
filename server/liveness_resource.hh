// liveness_resource.hh
// (C) Vince Coccia, 2015

#include <string>
#include <vector>
#include "rest_common.hh"

namespace rest
{
    struct AsyncLivenessResource
    {
        typedef std::shared_ptr<AsyncLivenessResource> ptr;
        static const std::string destination;
        static const std::string liveness_format;
        void operator()(const async_server::request&, async_server::connection_ptr);
    private:
        const std::vector<async_server::response_header> std_headers {
            {"Connection", "close"},
            {"Content-Type", "application/json"}
        };
    };
}