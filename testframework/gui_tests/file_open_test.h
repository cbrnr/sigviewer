#ifndef FILE_OPEN_TEST_H
#define FILE_OPEN_TEST_H

#include <QTest>
#include <QObject>

class FileOpenTest : public QObject
{
    Q_OBJECT
private slots:
    void testChannelSelection ();
};

#endif // FILE_OPEN_TEST_H
