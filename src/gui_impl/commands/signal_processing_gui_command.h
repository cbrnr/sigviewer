// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef SIGNAL_PROCESSING_GUI_COMMAND_H
#define SIGNAL_PROCESSING_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"
#include "gui_impl/dialogs/event_time_selection_dialog.h"

namespace SigViewer_
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

    static QString const MEAN_;
    static QString const POWER_SPECTRUM_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

} // namespace SigViewer_

#endif // SIGNAL_PROCESSING_GUI_COMMAND_H
