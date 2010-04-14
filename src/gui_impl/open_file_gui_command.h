#ifndef OPEN_FILE_GUI_COMMAND_H
#define OPEN_FILE_GUI_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"
#include "../file_handling/file_signal_reader.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// OpenFileGuiCommand
///
/// command for opening a file
class OpenFileGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    OpenFileGuiCommand ();

    //-------------------------------------------------------------------------
    static QSharedPointer<OpenFileGuiCommand> getInstance ();

    //-------------------------------------------------------------------------
    virtual void init ();

    //-------------------------------------------------------------------------
    void openFile (QString const& file_path);

public slots:
    //-------------------------------------------------------------------------
    virtual void trigger ();

private:
    static GuiActionFactoryRegistrator registrator_;

    //-------------------------------------------------------------------------
    QString showOpenDialog (QString const& path, QString const& extensions);

    //-------------------------------------------------------------------------
    QSharedPointer<FileSignalReader> createAndOpenFileSignalReader
            (QString const& file_path) const;


};

} // namespace BioSig_

#endif // OPEN_FILE_GUI_COMMAND_H
