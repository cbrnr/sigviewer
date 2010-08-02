#ifndef MAIN_WINDOW_MODEL_H
#define MAIN_WINDOW_MODEL_H

#include "signal_visualisation_model.h"
#include "../file_context.h"

namespace SigViewer_
{

class MainWindowModel
{
public:

    //-------------------------------------------------------------------------
    virtual ~MainWindowModel () {}

    //-------------------------------------------------------------------------
    virtual void closeCurrentFileTabs () = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> createSignalVisualisation (QString const& title, QSharedPointer<ChannelManager> channel_manager) = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> createSignalVisualisationOfFile (QSharedPointer<FileContext> file_ctx) = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> getCurrentSignalVisualisationModel () = 0;

};

}

#endif // MAIN_WINDOW_MODEL_H
