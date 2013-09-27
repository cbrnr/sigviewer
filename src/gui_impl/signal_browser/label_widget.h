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


#ifndef LABEL_WIDGET_H
#define LABEL_WIDGET_H

#include "base/sigviewer_user_types.h"
#include "gui/signal_view_settings.h"

#include <QMap>
#include <QWidget>
#include <QSharedPointer>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class LabelWidget : public QWidget
{
    Q_OBJECT
public:
    LabelWidget (QSharedPointer<SignalViewSettings const> signal_view_settings);

    void addChannel(ChannelID channel_nr, const QString& label);
    void removeChannel(ChannelID channel_nr);

public slots:
    void changeYStart (int32 y_start);

protected:
    void paintEvent(QPaintEvent* pe);
    virtual void contextMenuEvent (QContextMenuEvent* event);

private:
    QSharedPointer<SignalViewSettings const> signal_view_settings_;

    QMap<ChannelID, QString> channel_nr2label_;
    int32 y_start_;
};

} // namespace SigViewer_

#endif
