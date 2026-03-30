// © SigViewer developers
//
// License: GPL-3.0

#include "gui/color_manager.h"
#include "application_context.h"
#include "gui/gui_action_factory.h"
#include "gui/gui_action_factory_registrator.h"

#include <QApplication>
#include <QtTest>

using namespace sigviewer;

class TestColorManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        ApplicationContext::init({});
    }

    void cleanupTestCase()
    {
        ApplicationContext::cleanup();
    }

    void initialized()
    {
        QVERIFY(!ApplicationContext::getInstance()->getEventColorManager().isNull());
    }

    void defaultColor()
    {
        auto cm = ApplicationContext::getInstance()->getEventColorManager();
        QCOMPARE(cm->getDefaultEventColor(), QColor(200, 0, 0, 30));
    }

    void startOfTrialColor()
    {
        auto cm = ApplicationContext::getInstance()->getEventColorManager();
        QCOMPARE(cm->getDefaultEventColor(0x0300), QColor(0, 0, 200, 30));
    }
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("SigViewer");
    QApplication::setOrganizationDomain("http://github.com/cbrnr/sigviewer/");
    QApplication::setApplicationName("SigViewer");
    GuiActionFactoryRegistrator::registerActions();
    GuiActionFactory::getInstance()->initAllCommands();
    TestColorManager test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_color_manager.moc"
