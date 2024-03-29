// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef SIGNAL_PROCESSING_GUI_COMMAND_H
#define SIGNAL_PROCESSING_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"
#include "gui_impl/dialogs/event_time_selection_dialog.h"

namespace sigviewer
{

class SignalProcessingGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    SignalProcessingGuiCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private slots:
    //-------------------------------------------------------------------------
    void calculateMeanAndStandardDeviation ();

    //-------------------------------------------------------------------------
    void calculatePowerSpectrum ();

private:
    //-------------------------------------------------------------------------
    QSharedPointer<EventTimeSelectionDialog> getFinishedEventTimeSelectionDialog ();

    //-------------------------------------------------------------------------
    void createVisualisation (QString const& title, ChannelManager const& channel_manager);

    static QString const MEAN_();
    static QString const POWER_SPECTRUM_();
    static QStringList const ACTIONS_();

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // SIGNAL_PROCESSING_GUI_COMMAND_H
