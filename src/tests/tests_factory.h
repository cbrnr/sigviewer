#ifndef TESTS_FACTORY_H
#define TESTS_FACTORY_H

#include "test.h"
#include "data_block_tests.h"

#include <QSharedPointer>
#include <QList>

namespace BioSig_
{

namespace TestsFactory_
{

//-----------------------------------------------------------------------------
QList<QSharedPointer<Test> > allTests ()
{
    QList<QSharedPointer<Test> > tests;
    tests.append (QSharedPointer<Test>(new DataBlockTests));

    return tests;
}

}

}

#endif // TESTS_FACTORY_H
