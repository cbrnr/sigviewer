#ifndef EVENT_CONTEXT_MENU_H
#define EVENT_CONTEXT_MENU_H

#include <QMenu>

namespace BioSig_
{

namespace PortingToQT4_
{

//-----------------------------------------------------------------------------
///
/// EventContextMenu
///
/// enables editing of events... allows selection of events


class EventContextMenu : public QMenu
{
public:
    //-------------------------------------------------------------------------
    EventContextMenu ();

    //-------------------------------------------------------------------------
    virtual ~EventContextMenu ();

private:
    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    EventContextMenu (EventContextMenu const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    EventContextMenu& operator= (EventContextMenu const &);

};

}

}

#endif // EVENT_CONTEXT_MENU_H
