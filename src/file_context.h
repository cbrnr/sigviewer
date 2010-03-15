#ifndef FILE_CONTEXT_H
#define FILE_CONTEXT_H

#include <QObject>

namespace BioSig_
{

enum FileState
{
    FILE_STATE_UNCHANGED,
    FILE_STATE_CHANGED
};

//-----------------------------------------------------------------------------
///
/// FileContext
///
/// exists once per opened file...
///
class FileContext : public QObject
{
    Q_OBJECT
public:


    //-------------------------------------------------------------------------
    FileContext ();

    //-------------------------------------------------------------------------
    ~FileContext ();

    //-------------------------------------------------------------------------
    FileState getState () const;

signals:
    //-------------------------------------------------------------------------
    void stateChanged (FileState state);

public slots:
    //-------------------------------------------------------------------------
    void setState (FileState state);

private:
    //-------------------------------------------------------------------------
    // disabled
    FileContext (FileContext const&);
    FileContext& operator= (FileContext const&);

    FileState state_;
};

} // namespace BioSig_

#endif // FILE_CONTEXT_H
