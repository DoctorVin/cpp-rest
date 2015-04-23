// liveness_resource.cc
// (C) Vince Coccia, 2015
#include <ctime>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range.hpp>
#include <vector>
#include "liveness_resource.hh"

namespace rest
{
    const std::string AsyncLivenessResource::destination("/liveness");
    const std::string AsyncLivenessResource::liveness_format("{ proof_of_life : %1% }");
    
    void 
    AsyncLivenessResource::operator()(const async_server::request& req, async_server::connection_ptr conn)
    {
        std::vector<async_server::response_header> headers{std_headers};
        auto status = async_server::connection::not_supported;
        std::string payload{"{ error : \"Method not supported.\" }"};

        if (req.method == "GET") 
        {
            time_t now = ::time(nullptr);
            boost::format fmt{liveness_format};
            status = async_server::connection::ok;
            payload = (fmt % now).str();
        }

        headers.push_back({ "Content-Length", 
                            boost::lexical_cast<std::string>(payload.size()) });
        conn->set_status(status);
        conn->set_headers(boost::make_iterator_range(headers.begin(), headers.end()));
        conn->write(payload);
    }
}