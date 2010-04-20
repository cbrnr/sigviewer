#ifndef MAIN_WINDOW_MODEL_H
#define MAIN_WINDOW_MODEL_H

#include "signal_visualisation_model.h"
#include "../file_context.h"

namespace BioSig_
{

class MainWindowModel
{
public:

    //-------------------------------------------------------------------------
    virtual void closeCurrentFileTabs () = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> createSignalVisualisationOfFile (QSharedPointer<FileContext> file_ctx) = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<SignalVisualisationModel> getCurrentSignalVisualisationModel () = 0;

};

}

#endif // MAIN_WINDOW_MODEL_H
