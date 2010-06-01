#include "file_open_test.h"
#include "application_context_impl.h"
#include "gui/gui_action_factory.h"
#include "gui_impl/open_file_gui_command.h"
#include "gui_impl/dialogs/channel_selection_dialog.h"

#include <QApplication>
#include <QTest>

using namespace BioSig_;

void FileOpenTest::initTestCase()
{
    ApplicationContextImpl::init();
    GuiActionFactory::getInstance()->initAllCommands ();

    QVERIFY2(GuiActionFactory::getInstance()->getQAction("Save")->isEnabled() == false, "Saving is disabled!");
    QVERIFY2(GuiActionFactory::getInstance()->getQAction("Open...")->isEnabled(), "Open is enabled!");
    OpenFileGuiCommand::openFile ("/Users/eibel/Desktop/goa01.bkr", true);
}

void FileOpenTest::testChannelSelection ()
{
    QSharedPointer<SignalVisualisationModel> vis_model = ApplicationContextImpl::getInstance()->getMainWindowModel()->getCurrentSignalVisualisationModel();
    QSharedPointer<ChannelManager> channel_manager = ApplicationContextImpl::getInstance()->getCurrentFileContext()->getChannelManager();
    QVERIFY2(vis_model->getShownChannels().size() ==
             channel_manager->getNumberChannels(), "All channels are shown!");

    unsigned old_height = vis_model->getSignalHeight();
    float32 old_pixel_per_sample = vis_model->getPixelPerSample();
    GuiActionFactory::getInstance()->getQAction("Zoom In Vertical")->trigger();
    QVERIFY2(vis_model->getSignalHeight() > old_height, "Zoom In Vertical");
    QVERIFY2(vis_model->getPixelPerSample() == old_pixel_per_sample, "Pixel per sample have not changed");
}

void FileOpenTest::cleanupTestCase()
{
    ApplicationContextImpl::cleanup();
}

