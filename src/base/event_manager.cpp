#include "event_manager.h"
#include "file_signal_reader.h"
#include "file_signal_writer.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
EventManager::EventManager (FileSignalReader& reader, FileSignalWriter& writer)
    : reader_ (reader),
      writer_ (writer)
{

}

//-----------------------------------------------------------------------------
EventManager::~EventManager ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
std::set<EventID> EventManager::getEventsAt (unsigned pos,
                                             unsigned channel_id) const
{
    std::set<EventID> bla;
    return bla;
}

//-----------------------------------------------------------------------------
unsigned EventManager::getSampleRate () const
{
    return 0;
}

//-----------------------------------------------------------------------------
QString EventManager::getNameOfEventType (EventType type) const
{
    return QString("blub");
}

//-----------------------------------------------------------------------------
std::set<EventID> EventManager::getEventsOfType (EventType type) const
{
    std::set<EventID> bla;
    return bla;
}



}
