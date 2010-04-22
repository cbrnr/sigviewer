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
    virtual void init () = 0;

public slots:
    //-------------------------------------------------------------------------
    void setQActionEnabled (bool enabled)
    {
        emit qActionEnabledChanged (enabled);
    }

    //-------------------------------------------------------------------------
    virtual void trigger (QString const& action_name) = 0;

    //-------------------------------------------------------------------------
    virtual void applicationStateChanged (ApplicationState) {}

    //-------------------------------------------------------------------------
    virtual void tabSelectionStateChanged (TabSelectionState) {}

signals:
    //-------------------------------------------------------------------------
    void qActionEnabledChanged (bool enabled);

protected:
    //-------------------------------------------------------------------------
    GuiActionCommand (QStringList const& action_titles);

    //-------------------------------------------------------------------------
    QAction* getQAction (QString const& name);

private:
    QList<QAction*> actions_;
    QList<ActionConnector*> connectors_;
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
