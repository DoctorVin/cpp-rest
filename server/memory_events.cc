// memory_events.cc
// (C) Vince Coccia, 2015

#include "memory_events.hh"
#include <boost/uuid/uuid_io.hpp>

#include <memory>

namespace rest
{
    namespace util
    {
        using boost::uuids::uuid;
        
        MemoryEvents::MemoryEvents()
        {            
        }
        
        EventPersister::Ptr
        MemoryEvents::getPersister()
        {
            static EventPersister::Ptr ptr(new MemoryEvents);
            return ptr;
        }
        
        std::string
        MemoryEvents::create_event(const std::string& event_contents)
        {
            // assign a uuid as the id
            uuid event_id = id_gen();
            {
                std::lock_guard<std::mutex> lock(event_mutex);
                events.insert(std::make_pair(event_id, event_contents));
            }
            return boost::uuids::to_string(event_id);
        }
        
        void
        MemoryEvents::update_event(const std::string& idstr, const std::string& update)
        {
            uuid id;
            try
            {
                id = id_from_string(idstr);
                std::lock_guard<std::mutex> lock(event_mutex);
                auto item = events.find(id);
                if (item != events.end())
                {
                    events[id] = update;
                }
            }
            catch (std::runtime_error& ex)
            {
                throw PersistFailed(ex.what());
            }
            return;
        }
        
        std::string
        MemoryEvents::read_single_event(const std::string& idstr)
        {
            uuid id;
            std::string event{};
            try
            {
                id = id_from_string(idstr);
                std::lock_guard<std::mutex> lock(event_mutex);
                auto item = events.find(id);
                if (item != events.end())
                {
                    event = item->second;
                }
            }
            catch (std::runtime_error& ex)
            {
                if (std::string("invalid uuid string") != ex.what())
                    throw;
                //eat the invalid id exception and just return an empty string
            }
            return event;
        }
        
        void
        MemoryEvents::read_event_ids(EventList& list, uint32_t start_offset, uint32_t limit)
        {
            list.clear();
            std::lock_guard<std::mutex> lock(event_mutex);
            auto container_ptr = events.begin();
            auto container_end = events.end();
            while( container_ptr != container_end && 0 < start_offset)
            {
                ++container_ptr;
                --start_offset;
            }
            while (container_ptr != container_end && 0 < limit)
            {
                // TODO support filtering here
                list.insert(boost::uuids::to_string(container_ptr->first));
                ++container_ptr;
                --limit;
            }
            return;
        }
        
        void
        MemoryEvents::delete_event(const std::string& event_id)
        {
            uuid id;
            try
            {
                id = id_from_string(event_id);
                std::lock_guard<std::mutex> lock(event_mutex);
                auto item = events.find(id);
                if (item != events.end())
                {
                    events.erase(item);
                }
            }
            catch (std::runtime_error& ex)
            {
                if (std::string("invalid uuid string") != ex.what())
                    throw;
                //eat the invalid id exception and just return an empty string
            }
            return;
        }
    }
}