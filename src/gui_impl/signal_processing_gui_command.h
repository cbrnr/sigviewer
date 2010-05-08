#ifndef SIGNAL_PROCESSING_GUI_COMMAND_H
#define SIGNAL_PROCESSING_GUI_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"

namespace BioSig_
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

private:
    static QString const MEAN_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

} // namespace BioSig_

#endif // SIGNAL_PROCESSING_GUI_COMMAND_H
