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
