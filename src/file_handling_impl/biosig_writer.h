// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef BIOIGWRITER_H_
#define BIOIGWRITER_H_

#include "file_handling/file_signal_writer.h"

#include "biosig.h"


namespace sigviewer
{

//-----------------------------------------------------------------------------
class BioSigWriter : public FileSignalWriter
{
public:
    //-------------------------------------------------------------------------
    BioSigWriter ();

    //-------------------------------------------------------------------------
    virtual QPair<FileSignalWriter*, QString> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual ~BioSigWriter() {}

    //-------------------------------------------------------------------------
    virtual bool supportsSavingEvents () const;

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile (QSharedPointer<EventManager const>,
                                            std::set<EventType> const& types);

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<FileContext const> file_context,
                          std::set<EventType> const& types);

private:
    //-------------------------------------------------------------------------
    BioSigWriter (FileFormat target_type, QString new_file_path);

    FileFormat target_type_;
    QString new_file_path_;
    std::set<FileFormat> file_formats_support_event_saving_;
    mutable QMutex mutex_;
};

}

#endif
