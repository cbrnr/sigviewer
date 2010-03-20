#ifndef TAB_CONTEXT_H
#define TAB_CONTEXT_H

#include <QObject>

namespace BioSig_
{

enum TabSelectionState
{
    TAB_STATE_NO_EVENT_SELECTED,
    TAB_STATE_EVENT_SELECTED_ONE_CHANNEL,
    TAB_STATE_EVENT_SELECTED_ALL_CHANNELS
};

enum TabEditState
{
    TAB_STATE_READONLY,
    TAB_STATE_EDITABLE
};

class TabContext : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    TabContext ();

    //-------------------------------------------------------------------------
    ~TabContext ();

signals:
    //-------------------------------------------------------------------------
    void selectionStateChanged (TabSelectionState state);

    //-------------------------------------------------------------------------
    void editStateChanged (TabEditState state);

public slots:
    //-------------------------------------------------------------------------
    void setSelectionState (TabSelectionState state);

    //-------------------------------------------------------------------------
    void setEditState (TabEditState state);

private:
    //-------------------------------------------------------------------------
    // disabled
    TabContext (TabContext const&);
    TabContext& operator= (TabContext const&);

    TabSelectionState selection_state_;
    TabEditState edit_state_;
};

}

#endif // TAB_CONTEXT_H
