// request_context.cc
// (C) Vince Coccia, 2015

#include "request_context.hh"
#include <sstream>
namespace rest
{
    RequestContext::RequestContext(const string_type& input) : unparsed{input}
    {
        string_type::size_type terminator = input.find_first_of("?#");
        if (string_type::npos == terminator)
        {
            process_path(input);
        }
        else
        {
            process_path(input.substr(0, terminator));

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
    RequestContext::process_path(const string_type& path)
    {
        std::istringstream iss(path);
        std::ostringstream oss;
        if (iss.peek() != '/')
        {
            return;
        }
        else
        {
            iss.get();
        }
        while (! iss.eof())
        {
            char c;
            iss.get(c);
            switch(c)
            {
                // For right now I am complying rather strictly with RFC-3986 which stipulates that path segments
                // can be empty if there is nothing between 2 '/' or nothing after 1. This might not survive
                // into general usage.
                case '/':
                    path_segments.push_back(oss.str());
                    oss.str("");
                    break;
                default:
                    oss << c;
                    break;
            }
            if (std::char_traits<char>::eof() == iss.peek())
            {
                path_segments.push_back(oss.str());
            }
        }
    }
    
    void
    RequestContext::process_query(const string_type& query)
    {
        // now parse the query string ?key=value[&key2=value2&...]
        // note: according to RFC 3986 queries may contain additional '?' and '/' un-encoded
        // we're going to ignore both characters here and simply parse the string
        std::istringstream iss(query);
        std::ostringstream oss;
        
        //immediate sanity check, does this string begin with '?'
        if (iss.peek() != '?')
        {
            return;
        }
        else
        {
            iss.get();
        }
        enum StringState {KEY, VALUE};
        StringState s{StringState::KEY};
        string_type key, value;
        while ( ! iss.eof() )
        {
            char c;
            iss.get(c);
            switch(c)
            {
                case '=':
                    s = StringState::VALUE;
                    key = oss.str();
                    oss.str("");
                    break;
                case '&':
                    s = StringState::KEY;
                    value = oss.str();
                    oss.str("");
                    query_args[key] = value;
                    break;
                default:
                    oss << c;
                    break;
            }
            if (std::char_traits<char>::eof() == iss.peek())
            {
                if (StringState::KEY == s)
                {
                    key = oss.str();
                    // string ended while we were processing a key, clear any old value
                    value = string_type("");
                }
                else
                {
                    value = oss.str();
                }
                query_args[key] = value;
            }
        }
    }
}