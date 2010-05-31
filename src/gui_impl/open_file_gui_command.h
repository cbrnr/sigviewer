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
    static void openFile (QString file_path, bool instantly = false);

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private slots:
    //-------------------------------------------------------------------------
    void open ();

    //-------------------------------------------------------------------------
    void importEvents ();

    //-------------------------------------------------------------------------
    void showFileInfo ();

private:

    static QString const IMPORT_EVENTS_;
    static QString const OPEN_;
    static QString const SHOW_FILE_INFO_;
    static QStringList const ACTIONS_;


    static QSharedPointer<OpenFileGuiCommand> instance_;

    static GuiActionFactoryRegistrator registrator_;

    //-------------------------------------------------------------------------
    void openFileImpl (QString file_path, bool instantly = false);

    //-------------------------------------------------------------------------
    QString showOpenDialog (QString const& path, QString const& extensions);
};

} // namespace BioSig_

#endif // OPEN_FILE_GUI_COMMAND_H
