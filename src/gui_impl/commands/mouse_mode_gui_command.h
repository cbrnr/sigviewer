// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef MOUSE_MODE_GUI_COMMAND_H
#define MOUSE_MODE_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"
#include "gui/signal_visualisation_modes.h"

#include <QStringList>
#include <QActionGroup>

namespace sigviewer
{

class MouseModeGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    MouseModeGuiCommand ();
    virtual ~MouseModeGuiCommand ();

    virtual void init ();

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger (QString const& action_name);

protected:
    //-------------------------------------------------------------------------
    virtual void applicationStateChanged ();

    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private:
    static QString const NEW_MODE_TEXT_();
    static QString const POINTER_MODE_TEXT_();
    static QString const HAND_MODE_TEXT_();
    static QString const VIEW_OPTIONS_TEXT_();
    static QString const INFO_MODE_TEXT_();
    static QStringList const MODE_TEXTS_();

    static GuiActionFactoryRegistrator registrator_;

    QMap<QString, SignalVisualisationMode> action_to_mode_map_;
};

}

#endif // MOUSE_MODE_GUI_COMMAND_H
