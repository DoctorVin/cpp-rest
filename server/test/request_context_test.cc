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
    std::string invalid_fragment{"/some_resource#frag1#frag2"};
};

BOOST_FIXTURE_TEST_CASE(parse_root_resource, RequestContextFixture)
{
    RequestContext ctx(root_resource);
    BOOST_CHECK_EQUAL(root_resource, ctx.unparsed);
    BOOST_CHECK_EQUAL(0, ctx.path_segments.size());
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
    BOOST_CHECK_EQUAL(2, ctx.path_segments.size());
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

/*BOOST_FIXTURE_TEST_CASE(parse_good_long_query, RequestContextFixture)
{
    RequestContext ctx(good_long_query);
    BOOST_CHECK_EQUAL(good_long_query, ctx.unparsed);
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.resource);
    BOOST_CHECK_EQUAL(std::string{"sub_resource"}, ctx.sub_resource);
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
    BOOST_CHECK_EQUAL(std::string{"some_resource"}, ctx.resource);
    BOOST_CHECK_EQUAL(empty, ctx.sub_resource);
    BOOST_CHECK_EQUAL(std::string{"?key"}, ctx.query_string);
    BOOST_CHECK_EQUAL(empty, ctx.fragment);
    BOOST_CHECK_EQUAL(1, ctx.query_args.size());
    BOOST_CHECK_EQUAL(empty, ctx.query_args["key"]);
}*/

//BOOST_AUTO_TEST_CASE()