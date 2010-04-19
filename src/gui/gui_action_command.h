#ifndef GUI_ACTION_COMMAND_H
#define GUI_ACTION_COMMAND_H

#include "../base/application_states.h"

#include <QObject>
#include <QAction>

namespace BioSig_
{

class GuiActionCommand : public QObject
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    virtual ~GuiActionCommand () {}

    //-------------------------------------------------------------------------
    QAction* getQAction ();    

    //-------------------------------------------------------------------------
    virtual void init () = 0;

public slots:
    //-------------------------------------------------------------------------
    void setQActionEnabled (bool enabled)
    {
        emit qActionEnabledChanged (enabled);
    }

    //-------------------------------------------------------------------------
    virtual void trigger () = 0;

    //-------------------------------------------------------------------------
    virtual void applicationStateChanged (ApplicationState state) {}

signals:
    //-------------------------------------------------------------------------
    void qActionEnabledChanged (bool enabled);

protected:
    //-------------------------------------------------------------------------
    GuiActionCommand (QString const& name);

private:
    QAction* action_;
};


}


#endif // GUI_ACTION_COMMAND_H
