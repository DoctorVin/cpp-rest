// event-persister.hh
// (C) Vince Coccia, 2015

#pragma once
#include <stdexcept>
#include <string>
#include <set>

namespace rest
{
    namespace util
    {
        class PersistFailed : public std::runtime_error
        {
        public:
            PersistFailed(const std::string& what) : std::runtime_error(what) { }
        };
        
        struct EventPersister
        {
            typedef std::shared_ptr<EventPersister> Ptr;
            typedef std::set<std::string> EventList;
            virtual std::string create_event(const std::string& evt) = 0;
            virtual void update_event(const std::string& id, const std::string& evt) = 0;
            virtual std::string read_single_event(const std::string& id) = 0;
            virtual void read_event_ids(EventList&, uint32_t start_offset=0, uint32_t limit=25) = 0;
            virtual void delete_event(const std::string& id) = 0;
        };
    }
}