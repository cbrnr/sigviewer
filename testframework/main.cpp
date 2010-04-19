#include "base_tests/data_block_test.h"
#include "file_handling_tests/event_manager_test.h"

int main ()
{
    QTest::qExec(new EventManagerTest);
    QTest::qExec(new DataBlockTest);
}
