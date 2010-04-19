#ifndef CLOSE_FILE_GUI_COMMAND_H
#define CLOSE_FILE_GUI_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"
#include "../file_handling/file_signal_reader.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// CloseFileGuiCommand
///
/// command for closing a file
class CloseFileGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    CloseFileGuiCommand ();

    //-------------------------------------------------------------------------
    virtual ~CloseFileGuiCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

    //-------------------------------------------------------------------------
    void CloseFile (QString const& file_path);

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger ();

private:
    static GuiActionFactoryRegistrator registrator_;

    //-------------------------------------------------------------------------
    QString showCloseDialog (QString const& path, QString const& extensions);

    //-------------------------------------------------------------------------
    QSharedPointer<FileSignalReader> createAndCloseFileSignalReader
            (QString const& file_path) const;


};

} // namespace BioSig_


#endif // CLOSE_FILE_GUI_COMMAND_H
