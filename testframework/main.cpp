#include "base_tests/data_block_test.h"
#include "file_handling_tests/event_manager_test.h"
#include "gui_tests/file_open_test.h"
#include "application_context_impl.h"

#include <QSharedPointer>
#include <QApplication>

using namespace BioSig_;

int main (int32 argc, char* argv[])
{
    QApplication app (argc, argv);
    QTest::qExec(new EventManagerTest);
    QTest::qExec(new DataBlockTest);
    QTest::qExec(new FileOpenTest);
}
