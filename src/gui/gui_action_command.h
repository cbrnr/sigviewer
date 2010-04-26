#ifndef GUI_ACTION_COMMAND_H
#define GUI_ACTION_COMMAND_H

#include "../base/application_states.h"
#include "../base/tab_states.h"

#include <QObject>
#include <QAction>
#include <QStringList>

namespace BioSig_
{

class ActionConnector;

class GuiActionCommand : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    virtual ~GuiActionCommand () {}

    //-------------------------------------------------------------------------
    QList<QAction*> getQActions ();

    //-------------------------------------------------------------------------
    QList<QString> getActionIDs () const;

    //-------------------------------------------------------------------------
    virtual void init () = 0;

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger (QString const&) {}

    //-------------------------------------------------------------------------
    void updateEnablednessToApplicationState (ApplicationState state);

    //-------------------------------------------------------------------------
    void updateEnablednessToTabSelectionState (TabSelectionState state);

    //-------------------------------------------------------------------------
    QAction* getQAction (QString const& id);

signals:
    //-------------------------------------------------------------------------
    void qActionEnabledChanged (bool enabled);

protected:
    //-------------------------------------------------------------------------
    GuiActionCommand (QStringList const& action_ids);

    //-------------------------------------------------------------------------
    void resetActionTriggerSlot (QString const& action_id, const char* slot);

    //-------------------------------------------------------------------------
    virtual void applicationStateChanged () {}

    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness () {}

    //-------------------------------------------------------------------------
    ApplicationState getApplicationState () const {return app_state_;}

    //-------------------------------------------------------------------------
    TabSelectionState getTabSelectionState () const {return tab_sec_state_;}

private:
    QMap<QString, QAction*> action_map_;
    QList<ActionConnector*> connectors_;

    ApplicationState app_state_;
    TabSelectionState tab_sec_state_;
};

class ActionConnector : public QObject
{
    Q_OBJECT
public:
    ActionConnector (QObject* parent, QString const& name) : QObject (parent), name_ (name) {}
public slots:
    void trigger () {emit triggered (name_);}
signals:
    void triggered (QString const& name);
private:
    QString name_;
};




}


#endif // GUI_ACTION_COMMAND_H
