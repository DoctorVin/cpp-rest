// rest_processor.hh
// (C) Vince Coccia, 2015

#pragma once
#include "rest_common.hh"
#include <boost/thread/mutex.hpp>
#include <list>
#include <unordered_map>
#include <vector>

namespace rest
{    
    class WorkQueue 
    {
        boost::mutex listMtx;
        std::list<void_function> workList;
        boost::mutex mapMtx;
        std::unordered_map<std::string, resource_function> resources;
        const std::vector<async_server::response_header> std_headers {
            {"Connection", "close"},
            {"Content-Type", "application/json"}
        };
    public:
        void add_resource(const std::string&, const resource_function&);
        void operator()(const async_server::request&, async_server::connection_ptr);
        bool get_work(void_function&);
    };
}