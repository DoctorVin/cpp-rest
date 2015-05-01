// memory_events_test.cc
// (C) Vince Coccia, 2015

#include "memory_events.hh"
#include <vector>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE memory_events
#include <boost/test/unit_test.hpp>

using rest::util::MemoryEvents;
using rest::util::EventPersister;
using rest::util::PersistFailed;

struct MemoryEventsFixture
{
    std::vector<std::string> my_events { "This is my first event", "This is another event.", "Look! A THIRD EVENT!!"};
    EventPersister::Ptr p_persister;
    std::string bogus_event_id{"unknown event id"};
};

BOOST_AUTO_TEST_CASE(factory)
{
    EventPersister::Ptr p_persister;
    BOOST_CHECK(! p_persister);
    p_persister = MemoryEvents::getPersister();
    BOOST_CHECK(p_persister);
    EventPersister* p_ep = p_persister.get();
    p_persister.reset();
    BOOST_CHECK(! p_persister);
    p_persister = MemoryEvents::getPersister();
    BOOST_CHECK(p_persister);
    BOOST_CHECK_EQUAL(p_ep, p_persister.get());
}

BOOST_FIXTURE_TEST_SUITE(main_memory_events_suite, MemoryEventsFixture)

BOOST_AUTO_TEST_CASE(read_empty)
{
    EventPersister::EventList myList{"some bogus contents"};
    std::string event;
    p_persister = MemoryEvents::getPersister();
    BOOST_CHECK_NO_THROW(p_persister->read_event_ids(myList));
    BOOST_CHECK(myList.empty());
    BOOST_CHECK_NO_THROW(event = p_persister->read_single_event(bogus_event_id));
    BOOST_CHECK(event.empty());
}

BOOST_AUTO_TEST_CASE(CRUD)
{
    EventPersister::EventList myList;
    p_persister = MemoryEvents::getPersister();
    std::string event;
    const std::string update{"This is my updated event"};
    std::vector<std::string> event_ids; // event ids in order of creation
    // create
    BOOST_CHECK_NO_THROW(event_ids.push_back(p_persister->create_event(my_events[0])));
    BOOST_CHECK_NO_THROW(p_persister->read_event_ids(myList));
    BOOST_CHECK_EQUAL(1, myList.size());
    BOOST_CHECK_NO_THROW(event_ids.push_back(p_persister->create_event(my_events[0])));
    BOOST_CHECK_NO_THROW(p_persister->create_event(my_events[1]));
    // read
    BOOST_CHECK_NO_THROW(p_persister->read_event_ids(myList));
    BOOST_CHECK_EQUAL(3, myList.size());
    for( auto x: event_ids )
    {
        BOOST_CHECK(myList.end() != myList.find(x));
    }
    // update a bogus event
    BOOST_CHECK_THROW(p_persister->update_event(bogus_event_id, std::string{}), PersistFailed);
    // update a good event
    BOOST_CHECK_NO_THROW(p_persister->update_event(event_ids[1], update));
    BOOST_CHECK_NO_THROW(event = p_persister->read_single_event(event_ids[1]));
    BOOST_CHECK_EQUAL(event, update);
    // delete
    BOOST_CHECK_NO_THROW(p_persister->delete_event(event_ids[0]));
    // ensure a double delete is kosher
    BOOST_CHECK_NO_THROW(p_persister->delete_event(event_ids[0]));
    BOOST_CHECK_NO_THROW(p_persister->read_event_ids(myList));
    BOOST_CHECK_EQUAL(2, myList.size());
    
}

BOOST_AUTO_TEST_SUITE_END()