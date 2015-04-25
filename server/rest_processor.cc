// rest_processor.cc
// (C) Vince Coccia, 2015

#include "rest_processor.hh"
#include "request_context.hh"
#include <boost/thread/locks.hpp>
#include <boost/format.hpp>
#include <stdexcept>
#include <iostream>

namespace rest
{
    void
    WorkQueue::add_resource(const std::string& name, const resource_function& func)
    {
        boost::lock_guard<boost::mutex> lock(mapMtx);
        auto result = resources.insert(std::make_pair(name, func));
        if (! result.second)
        {
            boost::format err("Unable to insert a handler for resource %1%");
            throw std::runtime_error((err % name).str());
        }
    }
    
    bool
    WorkQueue::get_work(void_function& work)
    {
        bool something_to_do{false};
        boost::lock_guard<boost::mutex> lock(listMtx);
        if (! workList.empty())
        {
            work = workList.front();
            workList.pop_front();
            something_to_do = true;
        }
        return something_to_do;
    }
    
    std::string
    WorkQueue::parse_destination(const std::string& dest)
    {
        // strip any subresource-, query-, or anchor tags after the first slash
        std::string parsed{dest.substr(0, dest.find_first_of("/?#", 1))};
        return parsed;
    }
    
    void
    WorkQueue::operator()(const async_server::request& req, async_server::connection_ptr conn)
    {
        resource_function resFunc{};
        std::string payload{"{ error : \"Not found.\" }"};
        std::vector<async_server::response_header> headers{std_headers};
        
        std::string resource{parse_destination(req.destination)};
        
        std::cout << "Path: " << req.destination << std::endl;
        
        std::cout << "Resource: " << resource << std::endl;
        
        bool sendError{true};
        {
            boost::lock_guard<boost::mutex> lock(mapMtx);
            auto mapIterator = resources.find(resource);
            if (resources.end() != mapIterator)
            {
                resFunc = mapIterator->second;
                sendError = false;
            }
        }
        if (! sendError)
        {
            RequestContext ctx(req.destination);
            boost::lock_guard<boost::mutex> lock(listMtx);
            workList.push_back(std::bind(resFunc, std::cref(req), conn, ctx));
        }
        else
        {
            // no resource handler for what they requested, punt
            headers.push_back({ "Content-Length", 
                                boost::lexical_cast<std::string>(payload.size()) });
            conn->set_status(async_server::connection::not_found);
            conn->set_headers(boost::make_iterator_range(headers.begin(), headers.end()));
            conn->write(payload);
        }       
    }
}