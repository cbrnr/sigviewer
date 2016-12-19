// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef TEST_H
#define TEST_H

#include "file_handling/event_manager.h"
#include "gui/application_context.h"

#include <QSharedPointer>
#include <QObject>
#include <QString>

namespace sigviewer
{

namespace Tests_
{

#define VERIFY(condition, message) {if(!(condition)) return QString(message).append("; ").append(__FUNCTION__).append(":").append(QString::number(__LINE__)); else increasePassedTests();}
#define RUN_SUB_TEST(subtest) {QString result = subtest; if (result.size()) return result;}

//-----------------------------------------------------------------------------
/// Test
/// base class for all tests
class Test : public QObject
{
    Q_OBJECT
public:
    Test (QString const& name);

    QString getName () {return name_;}
    unsigned passed () {return passed_tests_;}

    virtual void init () {};

    virtual QString run () = 0;

    virtual void cleanup () {};
protected:
    void increasePassedTests () {passed_tests_++;}

    //-------------------------------------------------------------------------
    ChannelManager const& getChannelManagerDummyData ();

    //-------------------------------------------------------------------------
    QSharedPointer<EventManager> createEventManagerWithDummyData ();

    //-------------------------------------------------------------------------
    QSharedPointer<ApplicationContext> applicationContext ();

    //-------------------------------------------------------------------------
    QAction* action (QString const& action_name);

    //-------------------------------------------------------------------------
    QSharedPointer<SignalVisualisationModel> currentVisModel ();
private:
    QString name_;
    unsigned passed_tests_;
    QSharedPointer<ChannelManager const> channel_manager_;
};

}

}

#endif // TEST_H
