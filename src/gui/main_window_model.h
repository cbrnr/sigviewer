// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


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
