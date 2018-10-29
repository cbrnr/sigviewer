// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "save_gui_command.h"
#include "gui_impl/gui_helper_functions.h"
#include "file_handling/file_signal_writer_factory.h"
#include "open_file_gui_command.h"
#include "file_handling_impl/xdf_reader.h"
#include "fstream"

#include <QMessageBox>
#include <QFileDialog>
#include <QPaintEngine>
#include <QFile>
#include <QPainter>
#include <QPointer>

#include <algorithm>
#include <fstream>

namespace sigviewer
{

QString const SaveGuiCommand::SAVE_AS_ = "Save as...";
QString const SaveGuiCommand::SAVE_ = "Save";
QString const SaveGuiCommand::EXPORT_TO_PNG_ = "Export to PNG...";
QString const SaveGuiCommand::EXPORT_TO_GDF_ = "Export to GDF...";
QString const SaveGuiCommand::EXPORT_EVENTS_ = "Export Events...";

QStringList const SaveGuiCommand::ACTIONS_ = QStringList() <<
                                             SaveGuiCommand::SAVE_AS_ <<
                                             SaveGuiCommand::SAVE_ <<
                                             SaveGuiCommand::EXPORT_TO_GDF_ <<
                                             SaveGuiCommand::EXPORT_EVENTS_ <<
                                             SaveGuiCommand::EXPORT_TO_PNG_;



//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator SaveGuiCommand::registrator_ ("Saving",
                                                          QSharedPointer<SaveGuiCommand> (new SaveGuiCommand));


//-----------------------------------------------------------------------------
SaveGuiCommand::SaveGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void SaveGuiCommand::init ()
{
    setIcon(SAVE_, QIcon (":/images/baseline-save_alt-24px.svg"));
    setIcon(EXPORT_EVENTS_, QIcon (":/images/baseline-cloud_upload-24px.svg"));

    setShortcut (SAVE_, QKeySequence::Save);
    setShortcut (SAVE_AS_, QKeySequence::SaveAs);

    resetActionTriggerSlot (SAVE_AS_, SLOT(saveAs()));
    resetActionTriggerSlot (SAVE_, SLOT(save()));
    resetActionTriggerSlot (EXPORT_TO_PNG_, SLOT(exportToPNG()));
    resetActionTriggerSlot (EXPORT_TO_GDF_, SLOT(exportToGDF()));
    resetActionTriggerSlot (EXPORT_EVENTS_, SLOT(exportEvents()));
}


//-----------------------------------------------------------------------------
void SaveGuiCommand::saveAs ()
{
    QSharedPointer<FileContext> file_context = applicationContext()->getCurrentFileContext();
    QString old_file_path = file_context->getFilePath ();
    QString old_file_path_and_name = file_context->getFilePathAndName();
    QString file_name = file_context->getFileName();
    QString extension = file_name.mid(file_name.lastIndexOf('.'));
    extension = "*" + extension;

    QString new_file_path = GuiHelper::getFilePathFromSaveAsDialog (old_file_path,
                                                                    extension,
                                                                    tr("Signal files"));

    old_file_path_and_name.replace("\\", "/"); //change back slashes into forward slashes...

    if (new_file_path.size())
    {
        if (QFile::exists (new_file_path))
        {
            //if (QMessageBox::question(0, tr("Overwrite"), tr("Replace ") + new_file_path + tr("?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
            //else
            //  return;
            ///^There is already another dialog asking whether to overwrite: why do we need 2 dialogs? I commented it out for now

            if (old_file_path_and_name.compare(new_file_path) != 0) //if the old path is NOT the same as the new path
            {
                QFile::remove(new_file_path);
                QFile::copy (old_file_path_and_name, new_file_path);
            } //otherwise just leave it as is
        }
        else
            QFile::copy (old_file_path_and_name, new_file_path);

        //add save events to XDF support
        if (new_file_path.endsWith("xdf", Qt::CaseInsensitive))
        {
            XDFdata->writeEventsToXDF(new_file_path.toStdString());
            applicationContext()->getCurrentFileContext()->setState(FILE_STATE_UNCHANGED);
        }
        else
        {
            FileSignalWriter* writer (FileSignalWriterFactory::getInstance()->getHandler(new_file_path));

            bool can_save_events = false;
            if (writer)
                can_save_events = writer->supportsSavingEvents ();

            QSharedPointer<EventManager> event_mgr = file_context->getEventManager();
            if (writer && can_save_events)
            {
                QString error = writer->saveEventsToSignalFile (event_mgr, event_mgr->getEventTypes());
                if (error.size())
                    QMessageBox::critical(0, new_file_path, error);
                else
                {
                    file_context->resetFilePathAndName (new_file_path);
                    file_context->setState(FILE_STATE_UNCHANGED);
                }
            }
            else if (event_mgr->getNumberOfEvents() > 0)
                QMessageBox::information(0, "", "Events not stored to " + new_file_path + "\n If you want to store events export the file to GDF or export the events into a EVT file!");
            delete writer;
        }
    }
    else
        QMessageBox::critical(0, "Saving.... failed!", "Could not save " + file_name + " to " + new_file_path);
}

//-----------------------------------------------------------------------------
void SaveGuiCommand::save ()
{
    QString file_path = applicationContext()->getCurrentFileContext()->getFilePathAndName();
    QString file_name = applicationContext()->getCurrentFileContext()->getFileName();

    //add save events to XDF support
    if (file_path.endsWith("xdf", Qt::CaseInsensitive))
    {
        XDFdata->writeEventsToXDF(file_path.toStdString());
        applicationContext()->getCurrentFileContext()->setState(FILE_STATE_UNCHANGED);
    }
    else //Original Sigviewer code (not XDF)
    {
        FileSignalWriter* writer = FileSignalWriterFactory::getInstance()->getHandler (file_path);

        bool can_save_events = false;

        if (writer)
            can_save_events = writer->supportsSavingEvents();

        if (writer && can_save_events)
        {
            QSharedPointer<EventManager> event_mgr = applicationContext()->getCurrentFileContext()->getEventManager();
            QString error = writer->saveEventsToSignalFile(event_mgr, event_mgr->getEventTypes());
            if (error.size())
                QMessageBox::critical (0, tr("Error"), error);
            else
                applicationContext()->getCurrentFileContext()->setState(FILE_STATE_UNCHANGED);
        }
        else
        {
            QMessageBox::StandardButton pressed_button = QMessageBox::question (0, file_name, tr("Saving of Events is not possible to this file format! Do you want to convert this file into GDF?"),
                                                                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (pressed_button == QMessageBox::Yes)
                exportToGDF ();
        }
        delete writer;
    }
}

//-----------------------------------------------------------------------------
void SaveGuiCommand::exportToPNG ()
{
    QString extension = "*.png";
    QString file_path = GuiHelper::getFilePathFromSaveAsDialog (applicationContext()->getCurrentFileContext()->getFilePath(),
                                                                extension,
                                                                tr("PNG files"));

    SignalVisualisationView const* view = currentVisModel()->view();
    QSharedPointer<QImage> image = view->renderVisibleScene ();
    image->save (file_path, "PNG", 100);
}


//-----------------------------------------------------------------------------
void SaveGuiCommand::exportToGDF ()
{
    QMessageBox::information (0, "Information", "Attention: Converting to GDF is in testing phase. Meta-data will not be converted.");
    QString extensions = "*.gdf";
    QString current_file_path = applicationContext()->getCurrentFileContext()->getFilePathAndName();
    QString current_file_name = applicationContext()->getCurrentFileContext()->getFileName();
    current_file_path = current_file_path.left(current_file_path.size() - current_file_name.size());

    QString new_file_path = GuiHelper::getFilePathFromSaveAsDialog (current_file_path, extensions, tr("GDF files"));

    if (new_file_path.size() == 0)
        return;

    FileSignalWriter* writer = FileSignalWriterFactory::getInstance()->getHandler (new_file_path);
    if (writer == 0)
    {
        QMessageBox::critical (0, tr("Error"), tr("Export failed!"));
        return;
    }

    QSharedPointer<EventManager> event_mgr = currentVisModel()->getEventManager();

    QString error = writer->save (applicationContext()->getCurrentFileContext(), event_mgr->getEventTypes());
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

    delete writer;
}

//-------------------------------------------------------------------------
void SaveGuiCommand::exportEvents ()
{
    QString current_file_path = applicationContext()->getCurrentFileContext()->getFilePathAndName();

    QString extension = ".csv";
    QString extensions = "*.csv";

    QString new_file_path = GuiHelper::getFilePathFromSaveAsDialog
            (current_file_path.left(current_file_path.lastIndexOf('.')) +
             extension, extensions, tr("CSV files"));

    if (new_file_path.size() == 0)
        return;

    std::ofstream file;
    file.open(new_file_path.toStdString());

    if (file.is_open())
    {
        file << "position,duration,channel,type,name\n";

        QSharedPointer<EventManager> event_manager_pt = applicationContext()
                ->getCurrentFileContext()->getEventManager();

        struct row {
            unsigned long pos;
            unsigned long dur;
            int chan;
            int id;
            QString name;
        };

        QVector<row> events;

        for (unsigned int i = 0; i < event_manager_pt->getNumberOfEvents(); i++)
        {
            row tmp = {
                event_manager_pt->getEvent(i)->getPosition(),
                event_manager_pt->getEvent(i)->getDuration(),
                event_manager_pt->getEvent(i)->getChannel(),
                event_manager_pt->getEvent(i)->getType(),
                event_manager_pt->getNameOfEvent(i)
            };
            events.append(tmp);
        }

        std::sort(events.begin(),
                  events.end(),
                  [](const row& a, const row& b) {
                      return a.pos < b.pos;
                  }
        );

        for (int i = 0; i < events.size(); ++i)
        {
            file << events[i].pos << "," <<
                    events[i].dur << "," <<
                    events[i].chan << "," <<
                    events[i].id << "," <<
                    events[i].name.remove(",").toStdString() << "\n";
        }
        file.close();
    }
    else
    {
        QMessageBox::critical (0, current_file_path, tr("Exporting events to CSV failed!\nIs the target file open in another application?"));
    }
}

//-------------------------------------------------------------------------
void SaveGuiCommand::evaluateEnabledness ()
{
    disableIfNoFileIsOpened (QStringList() << EXPORT_TO_PNG_);
    disableIfNoSignalIsVisualised (QStringList() << EXPORT_TO_PNG_);
    bool file_open = getApplicationState () == APP_STATE_FILE_OPEN;
    bool no_gdf_file_open = false;
    bool file_changed = false;
    bool has_events = false;

    if (file_open)
    {
        no_gdf_file_open = !(applicationContext()->getCurrentFileContext()->getFileName().endsWith("gdf"));
        file_changed = (getFileState () == FILE_STATE_CHANGED);
        has_events = applicationContext()->getCurrentFileContext()->getEventManager()->getNumberOfEvents() > 0;

        if (applicationContext()->getCurrentFileContext()->getFileName().endsWith("xdf"))
            no_gdf_file_open = false;//Disabled because currently XDF to GDF conversion doesn't work
    }


    getQAction (SAVE_)->setEnabled (file_changed);
    getQAction (SAVE_AS_)->setEnabled (file_open);
    getQAction (EXPORT_TO_GDF_)->setEnabled (no_gdf_file_open);
    getQAction (EXPORT_EVENTS_)->setEnabled (has_events);
}

}
