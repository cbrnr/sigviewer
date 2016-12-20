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


#ifndef MAIN_WINDOW_MODEL_H
#define MAIN_WINDOW_MODEL_H

#include "signal_visualisation_model.h"
#include "event_view.h"
#include "file_context.h"

namespace sigviewer
{

class MainWindowModel
{
public:

    //-------------------------------------------------------------------------
    virtual ~MainWindowModel() {}

    //-------------------------------------------------------------------------
    virtual void closeCurrentFileTabs() = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> createSignalVisualisation(QString const& title, ChannelManager const& channel_manager) = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> createSignalVisualisationOfFile(QSharedPointer<FileContext> file_ctx) = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> getCurrentSignalVisualisationModel() = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventView> getCurrentEventView() = 0;
};

}

#endif // MAIN_WINDOW_MODEL_H
