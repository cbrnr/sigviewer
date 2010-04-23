#ifndef FIT_VIEW_GUI_COMMAND_H
#define FIT_VIEW_GUI_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"

#include <QStringList>

namespace BioSig_
{

class AdaptEventViewGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    AdaptEventViewGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~AdaptEventViewGuiCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger (QString const& action_name);

    //-------------------------------------------------------------------------
    virtual void applicationStateChanged (ApplicationState state);

    //-------------------------------------------------------------------------
    virtual void tabSelectionStateChanged (TabSelectionState state);
private:
    //-------------------------------------------------------------------------
    void fitViewToEvent ();

    //-------------------------------------------------------------------------
    void gotoAndSelectEvent (bool forward);

    //-------------------------------------------------------------------------
    void updateEnabledness ();

    ApplicationState app_state_;
    TabSelectionState tab_sec_state_;

    static QString const FIT_TO_EVENT_;
    static QString const GO_TO_NEXT_EVENT_;
    static QString const GO_TO_PREVIOUS_EVENT_;
    static QStringList const TEXTS_;

    static GuiActionFactoryRegistrator registrator_;

};

} // namespace BioSig_

#endif // FIT_VIEW_GUI_COMMAND_H
