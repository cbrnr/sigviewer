// © SigViewer developers
//
// License: GPL-3.0

#ifndef OPEN_FILE_GUI_COMMAND_H
#define OPEN_FILE_GUI_COMMAND_H

#include "file_handling/file_signal_reader.h"
#include "gui/dialogs/basic_header_info_dialog.h"
#include "gui/gui_action_command.h"
#include "gui/gui_action_factory_registrator.h"

namespace sigviewer {

/// OpenFileGuiCommand
///
/// command for opening a file
class OpenFileGuiCommand : public GuiActionCommand {
    Q_OBJECT
   public:
    OpenFileGuiCommand();

    virtual ~OpenFileGuiCommand();

    virtual void init();

    static void openFile(QString file_path);

    //! In case user has made changes to the old file but hasn't saved it, prompt the user to confirm whether to still close it
    bool confirmClosingOldFile();

   protected:
    virtual void evaluateEnabledness();

   private slots:

    void open();

    void importEvents();

    void showFileInfo();

   private:
    static QString const IMPORT_EVENTS_();
    static QString const OPEN_();
    static QString const SHOW_FILE_INFO_();
    static QStringList const ACTIONS_();

    static QSharedPointer<OpenFileGuiCommand> instance_();

    static GuiActionFactoryRegistrator registrator_;

    void openFileImpl(QString file_path, bool instantly = true);

    QString showOpenDialog(QString const& path, QString const& extensions);

    bool do_not_show_warning_message;

    QSharedPointer<BasicHeaderInfoDialog> basic_header_info_dialog;
};

}  // namespace sigviewer

#endif  // OPEN_FILE_GUI_COMMAND_H
