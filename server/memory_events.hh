// memory_events.hh
// (C) Vince Coccia, 2015

#pragma once
#include <mutex>
#include <unordered_map>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#include <boost/uuid/uuid.hpp>
#pragma clang diagnostic pop
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/functional/hash.hpp>
#include "event_persister.hh"

namespace rest
{
    namespace util
    {
        class MemoryEvents : public EventPersister
        {
            typedef boost::uuids::uuid event_id;
            std::unordered_map<event_id, std::string, boost::hash<boost::uuids::uuid>> events;
            boost::uuids::random_generator id_gen;
            boost::uuids::string_generator id_from_string;
            std::mutex event_mutex;
            MemoryEvents();
        public:
            static EventPersister::Ptr getPersister();
            
            // EventPersister i/f
            std::string create_event(const std::string& evt) override;
            void update_event(const std::string& id, const std::string& evt) override;
            std::string read_single_event(const std::string& id) override;
            void read_event_ids(EventList&, uint32_t start_offset=0, uint32_t limit=25) override;
            void delete_event(const std::string& id) override;
        };
    }
}