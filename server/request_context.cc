// request_context.cc
// (C) Vince Coccia, 2015

#include "request_context.hh"
// TODO: investigate a stateful parser here. This string-manipulation feels brittle
namespace rest
{
    RequestContext::RequestContext(const string_type& input) : unparsed{input}
    {
        string_type::size_type terminator = input.find_first_of("?#");
        string_type::size_type start = input.find_first_not_of("/");
        
        if (string_type::npos != start)
        {
            string_type path;
            string_type::size_type path_start{0};
            // restrict this string to just the section of interest
            if (string_type::npos == terminator)
            {
                path = input.substr(start, terminator);
            }
            else
            {
                path = input.substr(start, (terminator-start));
            }
            
            do 
            {
                string_type::size_type path_end = path.find("/", path_start);
                if (string_type::npos != path_end)
                {
                    //capture only up to the next '/'
                    path_segments.push_back(path.substr(path_start, (path_end-path_start)));
                    path_start = path.find_first_not_of("/", path_end);
                }
                else
                {
                    //capture the rest of the string
                    path_segments.push_back(path.substr(path_start, path_end));
                    path_start = path_end;
                }
            } while (string_type::npos != path_start);
        }        

        if (string_type::npos != terminator)
        {
            string_type::size_type query_mark = input.find_first_of("?");
            string_type::size_type fragment_mark = input.find_first_of("#");
            // don't attempt to handle '?' that appears after any '#'
            if (string_type::npos != query_mark && query_mark < fragment_mark)
            {
                query_string = input.substr(query_mark,
                                            (fragment_mark == string_type::npos) ? fragment_mark: (fragment_mark - query_mark));
                process_query(query_string);
            }
            if (string_type::npos != fragment_mark)
            {
                // fragment always goes from the initial '#' to the end of the string
                fragment = input.substr(fragment_mark, string_type::npos);
            }
        }
    }
    void
    RequestContext::process_query(const string_type& query)
    {
        // now parse the query string ?key=value[&key2=value2&...]
        // note: according to RFC 3986 queries may contain additional '?' and '/' un-encoded
        // we're going to ignore both characters here and simply parse the string
        
        //is there a key-value pair?
        string_type::size_type start = query.find_first_not_of("?");
        string_type::size_type eq = query.find("=");
        if (string_type::npos != eq)
        {
            // find an '&'
            string_type::size_type amp = query.find("&");
            if (string_type::npos != amp)
            {
                query_args[query.substr(start, eq-start)] = query.substr( query.find_first_not_of("=",eq), amp-eq);
                process_query(query.substr(amp));
            }
            else
            {
                query_args[query.substr(start, eq-start)] = query.substr(eq, amp);
            }
        }
        else
        {
            query_args[query] = string_type{};
        }
    }
}