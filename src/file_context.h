#ifndef FILE_CONTEXT_H
#define FILE_CONTEXT_H

#include <QObject>

namespace BioSig_
{

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
    enum State
    {
        UNCHANGED,
        CHANGED
    };

    //-------------------------------------------------------------------------
    FileContext ();

    //-------------------------------------------------------------------------
    ~FileContext ();

signals:
    //-------------------------------------------------------------------------
    void stateChanged (State state);

public slots:
    //-------------------------------------------------------------------------
    void setState (State state);

private:
    //-------------------------------------------------------------------------
    // disabled
    FileContext (FileContext const&);
    FileContext& operator= (FileContext const&);

    State state_;
};

} // namespace BioSig_

#endif // FILE_CONTEXT_H
