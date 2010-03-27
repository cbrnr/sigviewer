#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "../signal_browser/event_manager_interface.h"

namespace BioSig_
{

class FileSignalReader;
class FileSignalWriter;

class EventManager : public EventManagerInterface
{
public:
    EventManager (FileSignalReader& reader, FileSignalWriter& writer);
    virtual ~EventManager ();

    virtual std::set<EventID> getEventsAt (unsigned pos, unsigned channel_id) const;
    virtual unsigned getSampleRate () const;
    virtual QString getNameOfEventType (EventType type) const;
    virtual std::set<EventID> getEventsOfType (EventType type) const;

private:
    FileSignalReader& reader_;
    FileSignalWriter& writer_;
};

}

#endif // EVENT_MANAGER_H
