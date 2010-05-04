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
    virtual ~OpenFileGuiCommand ();

    //-------------------------------------------------------------------------
    virtual void init ();

    //-------------------------------------------------------------------------
    static void openFile (QString file_path);

private slots:
    //-------------------------------------------------------------------------
    void open ();

    //-------------------------------------------------------------------------
    void importEvents ();


private:
    static QString const IMPORT_EVENTS_;
    static QString const OPEN_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;

    //-------------------------------------------------------------------------
    QString showOpenDialog (QString const& path, QString const& extensions);

    //-------------------------------------------------------------------------
    static QSharedPointer<FileSignalReader> createAndOpenFileSignalReader
            (QString const& file_path);


};

} // namespace BioSig_

#endif // OPEN_FILE_GUI_COMMAND_H
