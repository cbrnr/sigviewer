// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef FILE_HANDLER_FACTORY_H
#define FILE_HANDLER_FACTORY_H

#include "base/exception.h"

#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>

#include <map>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// FileHandlerFactory
/// generic file handler factory for storing readers or writers or whatever :)
template<typename FileHandlerType>
class FileHandlerFactory
{
public:
    //-------------------------------------------------------------------------
    bool registerHandler (QString const& file_ending,
                          QSharedPointer<FileHandlerType> file_handler);

    //-------------------------------------------------------------------------
    void registerDefaultHandler (QSharedPointer<FileHandlerType> file_handler);

    //-------------------------------------------------------------------------
    FileHandlerType* getHandler (QString const& file_path);

    //-------------------------------------------------------------------------
    QStringList getAllFileEndingsWithWildcards () const;

protected :
    FileHandlerFactory () {}
    virtual ~FileHandlerFactory () {}

private:
    // not allowed
    FileHandlerFactory (const FileHandlerFactory& src);
    const FileHandlerFactory& operator=(const FileHandlerFactory& src);

    std::map<QString, QSharedPointer<FileHandlerType> > handler_map_;
    QSharedPointer<FileHandlerType> default_handler_;
    QStringList wildcard_file_endings_;
};

//-------------------------------------------------------------------------
template<typename FileHandlerType>
bool FileHandlerFactory<FileHandlerType>::registerHandler (QString const& file_ending,
                                                           QSharedPointer<FileHandlerType> file_handler)
{
    if (handler_map_.count (file_ending))
        return false;

    handler_map_[file_ending] = file_handler;
    qDebug () << "FileHandlerFactory<FileHandlerType>::registerHandler: file_ending = " << file_ending;
    wildcard_file_endings_ << QString("*.") + file_ending;
    return true;
}

//-------------------------------------------------------------------------
template<typename FileHandlerType>
void FileHandlerFactory<FileHandlerType>::registerDefaultHandler (QSharedPointer<FileHandlerType> file_handler)
{
    qDebug () << "FileHandlerFactory<FileHandlerType>::registerDefaultHandler";
    default_handler_ = file_handler;
}

//-------------------------------------------------------------------------
template<typename FileHandlerType>
FileHandlerType* FileHandlerFactory<FileHandlerType>::getHandler (QString const& file_path)
{
    QString file_ending = file_path.section('.', -1);
    qDebug () << "FACTORY " << file_ending;

    if (handler_map_.count(file_ending))
    {
        QPair<FileHandlerType*, QString> handler = handler_map_[file_ending]->createInstance (file_path);
        if (handler.first)
            return handler.first;
        else
        {
            handler = default_handler_->createInstance (file_path);
            if (handler.first)
                return handler.first;
            else
            {
                QMessageBox::information (0, QObject::tr("File Opening"), handler.second);
                return 0;
            }
        }
    }
    else if (!default_handler_.isNull())
    {
        QPair<FileHandlerType*, QString> handler = default_handler_->createInstance (file_path);
        if (handler.second.size())
            QMessageBox::information (0, QObject::tr("File Opening"), handler.second);

        return handler.first;
    }
    else
        return 0;
}

//-------------------------------------------------------------------------
template<typename FileHandlerType>
QStringList FileHandlerFactory<FileHandlerType>::getAllFileEndingsWithWildcards () const
{
    return wildcard_file_endings_;
}

} // namespace SigViewer_

#endif
