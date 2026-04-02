// © SigViewer developers
//
// License: GPL-3.0

#include <QApplication>
#include <QtTest>

#include "application_context.h"
#include "gui/color_manager.h"
#include "gui/gui_action_factory.h"
#include "gui/gui_action_factory_registrator.h"

using namespace sigviewer;

class TestColorManager : public QObject {
    Q_OBJECT

   private slots:
    void initTestCase() { ApplicationContext::init({}); }

    void cleanupTestCase() { ApplicationContext::cleanup(); }

    void initialized() {
        QVERIFY(!ApplicationContext::getInstance()->getEventColorManager().isNull());
    }

    void defaultColor() {
        auto cm = ApplicationContext::getInstance()->getEventColorManager();
        QCOMPARE(cm->getDefaultEventColor(), QColor(0, 0, 255, 20));
    }

    void startOfTrialColor() {
        auto cm = ApplicationContext::getInstance()->getEventColorManager();
        QCOMPARE(cm->getDefaultEventColor(0x0300), QColor(0, 85, 127, 20));
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setOrganizationName("SigViewer");
    QApplication::setOrganizationDomain("http://github.com/cbrnr/sigviewer/");
    QApplication::setApplicationName("SigViewer");
    GuiActionFactoryRegistrator::registerActions();
    GuiActionFactory::getInstance()->initAllCommands();
    TestColorManager test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_color_manager.moc"
