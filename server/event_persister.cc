// event_persister.cc
// (C) Vince Coccia, 2015

#include "event_persister.hh"

namespace rest
{
    namespace util
    {
        PersistFailed::PersistFailed(const std::string& what) : std::runtime_error(what)
        {
        }
    }
}