// event_resource.hh
// (C) Vince Coccia, 2015

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "rest_common.hh"
#include "request_context.hh"
#include "event_persister.hh"

namespace rest
{
    struct AsyncEventProcessor
    {
        static const std::string resource_target;
        AsyncEventProcessor(rest::util::EventPersister::Ptr);
        void operator()(const async_server::request&, async_server::connection_ptr, RequestContext);
    private:
        const rest::util::EventPersister::Ptr persister;
        const std::vector<async_server::response_header> std_headers {
            {"Connection", "close"},
            {"Content-Type", "application/json"}
        };
        
        void do_get(const async_server::request&, async_server::connection_ptr, const RequestContext&);
        void do_post(const async_server::request&, async_server::connection_ptr, const RequestContext&);
        void do_unsupported(const async_server::request&, async_server::connection_ptr);
    };
}