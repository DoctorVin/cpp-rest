// request_context_test.cc
// (C) Vince Coccia, 2015

#include "request_context.hh"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE request_context
#include <boost/test/unit_test.hpp>

using rest::RequestContext;
struct RequestContextFixture
{
    std::string empty;
    std::string root_resource{"/"};
    std::string plain_destination{"/some_resource"};
    std::string sub_resource_destination{"/some_resource/sub_resource/"};
    std::string good_query{"/some_resource?key=value"};
    std::string good_long_query{"/some_resource/sub_resource?key=val&key2=val2&key3=val3"};
    std::string incomplete_query{"/some_resource/?key"};
    std::string query_with_fragment{"/some_resource/sub_resource?key1=val1#fragment"};
    std::string good_fragment{"/some_resource#fragment"};
    std::string weird_fragment{"/some_resource#frag1#frag2"};
    std::string fragment_with_query{"/some_resource#fragment?with_unparsed_arg=true"};
};

BOOST_FIXTURE_TEST_CASE(parse_root_resource, RequestContextFixture)
{
    RequestContext ctx(root_resource);
    BOOST_CHECK_EQUAL(root_resource, ctx.unparsed);
    BOOST_CHECK_EQUAL(1, ctx.path_segments.size()); // this is an empty segment arising from '/'
    BOOST_CHECK_EQUAL(empty, ctx.query_string);
    BOOST_CHECK_EQUAL(empty, ctx.fragment);
    BOOST_CHECK_EQUAL(0, ctx.query_args.size());
}

BOOST_FIXTURE_TEST_CASE(parse_plain_destination, RequestContextFixture)
{
    RequestContext ctx(plain_destination);
    BOOST_CHECK_EQUAL(plain_destination, ctx.unparsed);
    BOOST_CHECK_EQUAL(1, ctx.path_segments.size());
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.path_segments[0]);
    BOOST_CHECK_EQUAL(empty, ctx.query_string);
    BOOST_CHECK_EQUAL(empty, ctx.fragment);
    BOOST_CHECK_EQUAL(0, ctx.query_args.size());   
}

BOOST_FIXTURE_TEST_CASE(parse_sub_resource_destination, RequestContextFixture)
{
    RequestContext ctx(sub_resource_destination);
    BOOST_CHECK_EQUAL(sub_resource_destination, ctx.unparsed);
    BOOST_CHECK_EQUAL(3, ctx.path_segments.size()); // the last segment is empty, arising from the terminal '/'
    BOOST_CHECK_EQUAL(std::string{"sub_resource"}, ctx.path_segments[1]);
    BOOST_CHECK_EQUAL(empty, ctx.query_string);
    BOOST_CHECK_EQUAL(empty, ctx.fragment);
    BOOST_CHECK_EQUAL(0, ctx.query_args.size());
}

BOOST_FIXTURE_TEST_CASE(parse_good_query, RequestContextFixture)
{
    RequestContext ctx(good_query);
    BOOST_CHECK_EQUAL(good_query, ctx.unparsed);
    BOOST_CHECK_EQUAL(1, ctx.path_segments.size());
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.path_segments[0]);
    BOOST_CHECK_EQUAL(std::string{"?key=value"}, ctx.query_string);
    BOOST_CHECK_EQUAL(empty, ctx.fragment);
    BOOST_CHECK_EQUAL(1, ctx.query_args.size());
    BOOST_CHECK_EQUAL(std::string{"value"}, ctx.query_args["key"]);
}

BOOST_FIXTURE_TEST_CASE(parse_good_long_query, RequestContextFixture)
{
    RequestContext ctx(good_long_query);
    BOOST_CHECK_EQUAL(good_long_query, ctx.unparsed);
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.path_segments[0]);
    BOOST_CHECK_EQUAL(std::string{"sub_resource"}, ctx.path_segments[1]);
    BOOST_CHECK_EQUAL(std::string{"?key=val&key2=val2&key3=val3"}, ctx.query_string);
    BOOST_CHECK_EQUAL(empty, ctx.fragment);
    BOOST_CHECK_EQUAL(3, ctx.query_args.size());
    BOOST_CHECK_EQUAL(std::string{"val"}, ctx.query_args["key"]);
    BOOST_CHECK_EQUAL(std::string{"val2"}, ctx.query_args["key2"]);
    BOOST_CHECK_EQUAL(std::string{"val3"}, ctx.query_args["key3"]);
}

BOOST_FIXTURE_TEST_CASE(parse_incomplete_query, RequestContextFixture)
{
    RequestContext ctx(incomplete_query);
    BOOST_CHECK_EQUAL(incomplete_query, ctx.unparsed);
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.path_segments[0]);
    BOOST_CHECK_EQUAL(2, ctx.path_segments.size());
    BOOST_CHECK_EQUAL(std::string{"?key"}, ctx.query_string);
    BOOST_CHECK_EQUAL(empty, ctx.fragment);
    BOOST_CHECK_EQUAL(1, ctx.query_args.size());
    BOOST_CHECK_EQUAL(empty, ctx.query_args["key"]);
}

BOOST_FIXTURE_TEST_CASE(parse_query_with_fragment, RequestContextFixture)
{
    RequestContext ctx(query_with_fragment);
    BOOST_CHECK_EQUAL(query_with_fragment, ctx.unparsed);
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.path_segments[0]);
    BOOST_CHECK_EQUAL(2, ctx.path_segments.size());
    BOOST_CHECK_EQUAL(std::string{"?key1=val1"}, ctx.query_string);
    BOOST_CHECK_EQUAL(std::string{"#fragment"}, ctx.fragment);
    BOOST_CHECK_EQUAL(1, ctx.query_args.size());
    BOOST_CHECK_EQUAL(std::string{"val1"}, ctx.query_args["key1"]);
}

BOOST_FIXTURE_TEST_CASE(parse_good_fragment, RequestContextFixture)
{
    RequestContext ctx(good_fragment);
    BOOST_CHECK_EQUAL(good_fragment, ctx.unparsed);
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.path_segments[0]);
    BOOST_CHECK_EQUAL(1, ctx.path_segments.size());
    BOOST_CHECK_EQUAL(empty, ctx.query_string);
    BOOST_CHECK_EQUAL(std::string{"#fragment"}, ctx.fragment);
    BOOST_CHECK_EQUAL(0, ctx.query_args.size());
}

BOOST_FIXTURE_TEST_CASE(parse_weird_fragment, RequestContextFixture)
{
    RequestContext ctx(weird_fragment);
    BOOST_CHECK_EQUAL(weird_fragment, ctx.unparsed);
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.path_segments[0]);
    BOOST_CHECK_EQUAL(1, ctx.path_segments.size());
    BOOST_CHECK_EQUAL(empty, ctx.query_string);
    BOOST_CHECK_EQUAL(std::string{"#frag1#frag2"}, ctx.fragment);
    BOOST_CHECK_EQUAL(0, ctx.query_args.size());
}

BOOST_FIXTURE_TEST_CASE(parse_fragment_with_query, RequestContextFixture)
{
    RequestContext ctx(fragment_with_query);
    BOOST_CHECK_EQUAL(fragment_with_query, ctx.unparsed);
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.path_segments[0]);
    BOOST_CHECK_EQUAL(1, ctx.path_segments.size());
    BOOST_CHECK_EQUAL(empty, ctx.query_string);
    BOOST_CHECK_EQUAL(std::string{"#fragment?with_unparsed_arg=true"}, ctx.fragment);
    BOOST_CHECK_EQUAL(0, ctx.query_args.size());
}
