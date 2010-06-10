#include "tests_dialog.h"
#include "tests_factory.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
TestsDialog::TestsDialog ()
{
    ui_.setupUi (this);
    ui_.progressBar->setValue (0);
}

//-----------------------------------------------------------------------------
void TestsDialog::runTests ()
{
    QList<QSharedPointer<Test> > tests = TestsFactory_::allTests ();
    ui_.progressBar->setMaximum (tests.size ());

    foreach (QSharedPointer<Test> test, tests)
    {
        QString test_name = test->getName();
        QString result = test->run();
        if (result.size())
            test_name += tr(" failed: ") + result;
        ui_.listWidget->addItem (test_name);
        ui_.progressBar->setValue(ui_.progressBar->value()+1);
    }

}

//-----------------------------------------------------------------------------
void TestsDialog::on_start_button__clicked ()
{
    runTests();
}


}
