#ifndef TAB_CONTEXT_H
#define TAB_CONTEXT_H

#include <QObject>

namespace BioSig_
{

class TabContext : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        NO_EVENTS_POSSIBLE,
        NO_EVENT_SELECTED,
        EVENT_SELECTED_ONE_CHANNEL,
        EVENT_SELECTED_ALL_CHANNELS
    };

    //-------------------------------------------------------------------------
    TabContext ();

    //-------------------------------------------------------------------------
    ~TabContext ();

signals:
    //-------------------------------------------------------------------------
    void stateChanged (TabContext::State state);

public slots:
    //-------------------------------------------------------------------------
    void setState (TabContext::State state);

private:
    //-------------------------------------------------------------------------
    // disabled
    TabContext (TabContext const&);
    TabContext& operator= (TabContext const&);

    State state_;
};

}

#endif // TAB_CONTEXT_H
