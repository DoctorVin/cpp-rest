// rest_common.hh
// (C) Vince Coccia, 2015

#pragma once
#include <boost/network/include/http/server.hpp> // there has to be a way to avoid including the world...
#include <functional>
#include <memory>

namespace rest
{
    class WorkQueue;
    typedef boost::network::http::async_server<WorkQueue> async_server;
    
    //convenience typedefs for this namespace
    typedef boost::function<void()> void_function;
    typedef std::shared_ptr<void_function> void_function_ptr;
    typedef boost::function<void(const async_server::request&, async_server::connection_ptr)> resource_function;
    typedef std::shared_ptr<resource_function> resource_function_ptr;
}