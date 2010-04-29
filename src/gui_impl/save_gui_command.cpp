#include "save_gui_command.h"
#include "gui_helper_functions.h"
#include "../file_handling/file_signal_writer_factory.h"
#include "../application_context.h"
#include "open_file_gui_command.h"

#include <QMessageBox>
#include <QFile>

namespace BioSig_
{

QString const SaveGuiCommand::SAVE_AS_ = "Save as...";
QString const SaveGuiCommand::SAVE_ = "Save";
QString const SaveGuiCommand::EXPORT_TO_GDF_ = "Export to GDF...";
QString const SaveGuiCommand::EXPORT_EVENTS_ = "Export Events...";


QStringList const SaveGuiCommand::ACTIONS_ = QStringList() <<
                                             SaveGuiCommand::SAVE_AS_ <<
                                             SaveGuiCommand::SAVE_ <<
                                             SaveGuiCommand::EXPORT_TO_GDF_ <<
                                             SaveGuiCommand::EXPORT_EVENTS_;



//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator SaveGuiCommand::registrator_ ("Saving",
                                                          QSharedPointer<SaveGuiCommand> (new SaveGuiCommand));


//-----------------------------------------------------------------------------
SaveGuiCommand::SaveGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{

}

//-----------------------------------------------------------------------------
void SaveGuiCommand::init ()
{
    resetActionTriggerSlot (SAVE_AS_, SLOT(saveAs()));
    resetActionTriggerSlot (SAVE_, SLOT(save()));
    resetActionTriggerSlot (EXPORT_TO_GDF_, SLOT(exportToGDF()));
    resetActionTriggerSlot (EXPORT_EVENTS_, SLOT(exportEvents()));
}


//-----------------------------------------------------------------------------
void SaveGuiCommand::saveAs ()
{
    QString old_file_path = ApplicationContext::getInstance()->getCurrentFileContext()->getFilePath ();
    QString old_file_path_and_name = ApplicationContext::getInstance()->getCurrentFileContext()->getFilePathAndName();
    QString file_name = ApplicationContext::getInstance()->getCurrentFileContext()->getFileName();
    QString extension = file_name.mid(file_name.lastIndexOf('.'));
    extension = "*" + extension;

    QString new_file_path = GuiHelper::getFilePathFromSaveAsDialog (old_file_path,
                                                                    extension);

    if (new_file_path.size())
    {
        if (QFile::copy (old_file_path_and_name, new_file_path))
        {
            FileSignalWriter* writer = FileSignalWriterFactory::getInstance()->getElement(new_file_path.mid(new_file_path.lastIndexOf('.')));

            bool can_save_events = false;
            if (writer)
                can_save_events = writer->supportsSavingEvents ();

            QSharedPointer<EventManager> event_mgr = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
            if (writer && can_save_events)
            {
                QString error = writer->saveEventsToSignalFile (event_mgr, new_file_path);
                delete writer;
                if (error.size())
                    QMessageBox::critical(0, new_file_path, error);
            }
            else if (event_mgr->getNumberOfEvents() > 0)
                QMessageBox::information(0, "", "Events not stored to " + new_file_path + "\n If you want to store events export the file to GDF or export the events into a EVT file!");
        }
        else
            QMessageBox::critical(0, "Saving.... failed!", "Could not save " + file_name + " to " + new_file_path);
    }
}

//-----------------------------------------------------------------------------
void SaveGuiCommand::save ()
{
    QString file_path = ApplicationContext::getInstance()->getCurrentFileContext()->getFilePathAndName();
    QString file_name = ApplicationContext::getInstance()->getCurrentFileContext()->getFileName();
    FileSignalWriter* writer = FileSignalWriterFactory::getInstance()->getElement(file_name.mid(file_name.lastIndexOf('.')));

    bool can_save_events = false;

    if (writer)
        can_save_events = writer->supportsSavingEvents();

    if (writer && can_save_events)
    {
        QSharedPointer<EventManager> event_mgr = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
        writer->saveEventsToSignalFile(event_mgr, file_path);
        delete writer;
    }
    else
    {
        if (writer)
            delete writer;
        QMessageBox::StandardButton pressed_button = QMessageBox::question (0, file_name, tr("Saving of Events is not possible to this file format! Do you want to convert this file into GDF?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (pressed_button == QMessageBox::Yes)
            exportToGDF ();
    }
}

//-----------------------------------------------------------------------------
void SaveGuiCommand::exportToGDF ()
{
    QString extensions = "*.gdf";
    QString current_file_path = ApplicationContext::getInstance()->getCurrentFileContext()->getFilePathAndName();
    QString current_file_name = ApplicationContext::getInstance()->getCurrentFileContext()->getFileName();
    current_file_path = current_file_path.left(current_file_path.size() - current_file_name.size());

    QString new_file_path = GuiHelper::getFilePathFromSaveAsDialog (current_file_path, extensions);

    if (new_file_path.size() == 0)
        return;

    FileSignalWriter* writer = FileSignalWriterFactory::getInstance()->getElement(".gdf");

    QSharedPointer<EventManager> event_mgr = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();

    QString error = writer->save (event_mgr, ApplicationContext::getInstance()->getCurrentFileContext()->getFilePathAndName(), new_file_path);

    delete writer;

    if (error.size() == 0)
    {
        QMessageBox::StandardButton pressed_button = QMessageBox::question(0, current_file_name, current_file_name + tr(" has been converted into GDF and stored in:\n") + new_file_path
                                                                           + tr("\n\nDo you want to open the GDF file?"),
                                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (pressed_button == QMessageBox::Yes)
            OpenFileGuiCommand::openFile (new_file_path);
    }
    else
        QMessageBox::critical (0, current_file_name, tr("Exporting to GDF failed!\n") + error);

}

//-------------------------------------------------------------------------
void SaveGuiCommand::exportEvents ()
{
    /// waldesel: SHOW EVENT TYPE DIALOG to choose event types that should be exported!!

    QString file_path = ApplicationContext::getInstance()->getCurrentFileContext()->getFilePathAndName();

    QString new_file_ending = ".evt";
    QString new_file_path = file_path;
    new_file_path.replace(file_path.lastIndexOf('.'), new_file_ending.length(), new_file_ending);

    FileSignalWriter* file_signal_writer = FileSignalWriterFactory::getInstance()
                                           ->getElement(new_file_ending);

    file_signal_writer->save (ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager(),
                              file_path,
                              new_file_path);
    delete file_signal_writer;
}



}
