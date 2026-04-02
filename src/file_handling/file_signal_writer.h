// © SigViewer developers
//
// License: GPL-3.0

#ifndef FILE_SIGNAL_WRITER_H
#define FILE_SIGNAL_WRITER_H

#include "base/signal_event.h"
#include "channel_manager.h"
#include "event_manager.h"
#include "file_context.h"
#include "file_signal_reader.h"

namespace sigviewer {

// abstract signal writer
class FileSignalWriter {
   public:
    virtual ~FileSignalWriter() {}

    //-------------------------------------------------------------------------
    virtual QPair<FileSignalWriter*, QString> createInstance(QString const& file_path) = 0;

    //-------------------------------------------------------------------------
    virtual bool supportsSavingEvents() const { return false; }

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile(QSharedPointer<EventManager const>,
        std::set<EventType> const& types) = 0;

    //-------------------------------------------------------------------------
    virtual QString save(QSharedPointer<FileContext const> file_context) {
        QSharedPointer<EventManager const> event_manager = file_context->getEventManager();
        if (event_manager.isNull())
            return save(file_context, std::set<EventType>());
        else
            return save(file_context, event_manager->getEventTypes(""));
    }

    //-------------------------------------------------------------------------
    virtual QString save(QSharedPointer<FileContext const> file_context,
        std::set<EventType> const& types) = 0;

   protected:
    FileSignalWriter() {}

   private:
    Q_DISABLE_COPY(FileSignalWriter)
};

}  // namespace sigviewer

#endif
