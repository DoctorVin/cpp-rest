// request_context.cc
// (C) Vince Coccia, 2015

#include "request_context.hh"

namespace rest
{
    RequestContext::RequestContext(const string_type& input) : unparsed{input}
    {
        resource = input.substr(0, input.find_first_of("/", 1));
        sub_resource = input.substr(resource.size(), input.find_first_of("?#"));
        string_type::size_type query_mark = input.find_first_of("?");
        string_type::size_type fragment_mark = input.find_first_of("#");
        if (string_type::npos != query_mark)
        {
            query_string = input.substr(query_mark, fragment_mark);
        }
        if (string_type::npos != fragment_mark)
        {
            fragment = input.substr(fragment_mark, string_type::npos);
        }
        
        // now parse the query string ?key=value[&key2=value2&...]
        string_type::size_type amp{1};
        string_type::size_type eq = query_string.find_first_of("=");
        while (string_type::npos != eq)
        {
            string_type key, val;

            string_type::size_type key_len{eq - amp};
            key = query_string.substr(amp, key_len);
            
            amp = query_string.find_first_of("&");
            string_type::size_type val_len;
            if (string_type::npos == amp)
            {
                val_len = query_string.size() - eq;
            }
            else
            {
                val_len = amp - eq;
            }
            val = query_string.substr(eq, val_len);
            query_args[key] = val;
            eq = query_string.find_first_of("=", amp);
        }
    }
}