#ifndef CLOSE_FILE_GUI_COMMAND_H
#define CLOSE_FILE_GUI_COMMAND_H

#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"
#include "file_handling/file_signal_reader.h"

namespace SigViewer_
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
    virtual ~CloseFileGuiCommand () {}

    //-------------------------------------------------------------------------
    virtual void init ();

    //-------------------------------------------------------------------------
    bool closeCurrentFile ();

public slots:
    //-------------------------------------------------------------------------
    void closeFile ();

    //-------------------------------------------------------------------------
    void exitApplication ();


protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();


private:
    static QString const CLOSE_FILE_;
    static QString const EXIT_APPLICATION_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;



    //-------------------------------------------------------------------------
    QString showCloseDialog (QString const& path, QString const& extensions);

    //-------------------------------------------------------------------------
    QSharedPointer<FileSignalReader> createAndCloseFileSignalReader
            (QString const& file_path) const;


};

} // namespace SigViewer_


#endif // CLOSE_FILE_GUI_COMMAND_H
