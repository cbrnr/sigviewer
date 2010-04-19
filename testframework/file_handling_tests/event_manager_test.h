#ifndef EVENT_MANAGER_TEST_H
#define EVENT_MANAGER_TEST_H

#include "file_handling/event_manager.h"

#include <QTest>
#include <QObject>


class EventManagerTest : public QObject
{
    Q_OBJECT
private slots:
    void init ();

    void testCreatingEvents ();

    void cleanup ();

private:
    BioSig_::EventManager* event_manager_;
};

#endif // EVENT_MANAGER_TEST_H
