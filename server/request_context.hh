// request_context.hh
// (C) Vince Coccia, 2015

#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace rest
{
    struct RequestContext
    {
        typedef std::string string_type;
        RequestContext(const string_type&);
        string_type unparsed;
        string_type query_string;
        string_type fragment;
        std::vector<string_type> path_segments;
        std::unordered_map<string_type, string_type> query_args;
    private:
        void process_query(const string_type& query);
    };
}