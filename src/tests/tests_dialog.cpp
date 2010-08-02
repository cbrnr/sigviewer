#include "tests_dialog.h"
#include "tests_factory.h"
#include "gui_impl/commands/open_file_gui_command.h"

namespace SigViewer_
{

using namespace Tests_;

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
    ui_.progressBar->setValue (0);

    unsigned fails = 0;

    foreach (QSharedPointer<Test> test, tests)
    {
        QColor color;
        QString test_name = test->getName();
        test->init();
        QString result = test->run();
        test->cleanup();
        if (result.size())
        {
            test_name += tr(" failed: ") + result;
            color = QColor (255, 0, 0, 50);
            fails++;
        }
        else
        {
            test_name += tr(": passed ") + QString::number(test->passed()) + tr(" verifications");
            color = QColor (0, 255, 0, 50);
        }
        ui_.listWidget->addItem (test_name);
        ui_.listWidget->item (ui_.listWidget->count() - 1)->setBackgroundColor (color);
        ui_.progressBar->setValue(ui_.progressBar->value()+1);
    }
    if (fails)
        ui_.listWidget->addItem(QString::number(fails) + " tests failed!");
    else
        ui_.listWidget->addItem("All tests passed!");
}

//-----------------------------------------------------------------------------
void TestsDialog::on_start_button__clicked ()
{
    runTests();
}

//-----------------------------------------------------------------------------
void TestsDialog::on_open_dummy__clicked ()
{
    OpenFileGuiCommand::openFile ("blub.sinusdummy", true);
    close();
}


}
