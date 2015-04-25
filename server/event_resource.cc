// event_resource.cc
//(C) Vince Coccia, 2015

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range.hpp>
#include "event_resource.hh"

namespace rest
{
    using util::EventPersister;
    using util::PersistFailed;
    
    const std::string AsyncEventProcessor::resource_target{"/events"};
    
    // Public API
    AsyncEventProcessor::AsyncEventProcessor(EventPersister::Ptr ptr) : persister{ptr}
    {
    }
    
    void
    AsyncEventProcessor::operator()(const async_server::request& req, async_server::connection_ptr conn, RequestContext ctx)
    {
        if ("POST" == req.method)
            do_post(req, conn, ctx);
        else if ("GET" == req.method || "HEAD" == req.method)
            do_get(req, conn, ctx);
        else
        {
            // DELETE, PUT, OPTIONS, PATCH, GIGO
            do_unsupported(req, conn);
        }
    }
    
    // Private API
    void
    AsyncEventProcessor::do_unsupported(const async_server::request& req, async_server::connection_ptr conn)
    {
        std::vector<async_server::response_header> headers{std_headers};
        auto status = async_server::connection::not_supported;
        std::string payload{"{ error : \"Method not supported.\" }"};
        headers.push_back({ "Content-Length", 
                            boost::lexical_cast<std::string>(payload.size()) });
        conn->set_status(status);
        conn->set_headers(boost::make_iterator_range(headers.begin(), headers.end()));
        conn->write(payload);        
    }
    
    void
    AsyncEventProcessor::do_get(const async_server::request& req, async_server::connection_ptr conn, const RequestContext& ctx)
    {
        std::vector<async_server::response_header> headers{std_headers};
        auto status = async_server::connection::not_found;
        std::string payload{"{ error: \"Event not found.\""};
        if (ctx.sub_resource.size())
        {
            // the request is for a single event, ignore any query
            std::string event = persister->read_single_event(ctx.sub_resource);
            if (event.size())
            {
                status = async_server::connection::ok;
                payload = event;
            }
        }
        else
        {
            // when we get a request for the event collection, return a list of links to individual events
            boost::format link_format("http://%1%/%2%/%3%");
            boost::format collection_format{"{ events : [ %1% ] }"};
            boost::format atom_format{"{ id : \"%1%\", link : \"%2%\" }"};
            EventPersister::EventList list;
            
            uint32_t page_size{20};
            std::string event_id_filter{};
            uint32_t start_offset{0};
            // find any interesting query parameters
            auto query_end = ctx.query_args.end();
            auto query_page_size = ctx.query_args.find("page_size");
            auto query_event_filter = ctx.query_args.find("event_filter");
            auto query_start_offset = ctx.query_args.find("start_offset");
            if (query_end != query_page_size)
            {
                try
                {
                    page_size = boost::lexical_cast<uint32_t>(query_page_size->second);
                }
                catch (boost::bad_lexical_cast&)
                {
                    // just eat the exception and use the default
                }
            }
            if (query_end != query_event_filter)
            {
                event_id_filter = query_event_filter->second;
            }
            if (query_end != query_start_offset)
            {
                try
                {
                    start_offset = boost::lexical_cast<uint32_t>(query_start_offset->second);
                }
                catch (boost::bad_lexical_cast&)
                {
                    // we have a sensible default
                }
            }
            
            persister->read_event_ids(list, start_offset, page_size, event_id_filter);
            if (list.size())
            {
                status = async_server::connection::ok;
                std::vector<std::string> atoms;
                BOOST_FOREACH(auto el, list)
                {
                    std::string this_link = (link_format % req.source % resource_target % el).str();
                    std::string this_atom = (atom_format % el % this_link).str();
                    atoms.push_back(this_atom);
                }
                payload = ( collection_format % boost::algorithm::join(atoms, ",") ).str();
            }           
        }
        headers.push_back({ "Content-Length", 
                            boost::lexical_cast<std::string>(payload.size()) });
        conn->set_status(status);
        conn->set_headers(boost::make_iterator_range(headers.begin(), headers.end()));
        conn->write(payload);
    }
    
    void
    AsyncEventProcessor::do_post(const async_server::request& req, async_server::connection_ptr conn, const RequestContext& ctx)
    {
        // sanity check incoming headers
        // only accept something that says it's application/json (we'll validate in the future sometime)
        bool send_unsupported{true};
        std::vector<async_server::response_header> headers{std_headers};
        std::string payload{"{ error : \"Internal Server Error\" "};
        auto status = async_server::connection::internal_server_error;
        for(auto h : req.headers)
        {
            if ("Content-Type" == h.name && "application/json" == h.value)
            {
                send_unsupported = false;
                break;
            }
        }
        if (send_unsupported)
        {
            do_unsupported(req, conn);
        }
        else
        {
            std::string event = body(req);
            try
            {
                auto id = persister->create_event(event);
                payload = (boost::format("{ id : \"%1%\" }") % id).str();
                status = async_server::connection::created;
            }
            catch (PersistFailed& ex)
            {
                // oops, log the error or do something else useful.
                // this is not necessarily fatal
            }
        }
        conn->set_status(status);
        if (payload.size())
        {
            headers.push_back({ "Content-Length", 
                                boost::lexical_cast<std::string>(payload.size()) });
        }
        conn->set_headers(boost::make_iterator_range(headers.begin(), headers.end()));
        conn->write(payload);
    }
}