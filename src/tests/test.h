#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QString>

namespace BioSig_
{

#define VERIFY(condition, message) if(!condition) return message;

//-----------------------------------------------------------------------------
/// Test
/// base class for all tests
class Test : public QObject
{
    Q_OBJECT
public:
    Test (QString const& name) : name_ (name) {}

    QString getName () {return name_;}

    virtual QString run () = 0;

private:
    QString name_;
};

}

#endif // TEST_H
