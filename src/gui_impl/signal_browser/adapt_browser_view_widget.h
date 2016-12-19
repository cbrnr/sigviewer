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


#ifndef ADAPT_BROWSER_VIEW_WIDGET_H
#define ADAPT_BROWSER_VIEW_WIDGET_H

#include "gui/signal_visualisation_view.h"
#include "gui/signal_view_settings.h"
#include "ui_adapt_browser_view_widget.h"

#include <QWidget>
#include <QSharedPointer>

namespace sigviewer
{

//-------------------------------------------------------------------------
class AdaptBrowserViewWidget : public QWidget
{
Q_OBJECT
public:
    //-------------------------------------------------------------------------
    explicit AdaptBrowserViewWidget (SignalVisualisationView const* signal_visualisation_view,
                                     QSharedPointer<SignalViewSettings> settings_,
                                     QWidget *parent = 0);
signals:
    //-------------------------------------------------------------------------
    void xAxisVisibilityChanged (bool visible);

    //-------------------------------------------------------------------------
    void yAxisVisibilityChanged (bool visible);

    //-------------------------------------------------------------------------
    void labelsVisibilityChanged (bool visible);
private slots:
    //-------------------------------------------------------------------------
    void on_channelOverlappingSlider_valueChanged (int value);

    //-------------------------------------------------------------------------
    void on_yGridSlider_valueChanged (int value);

    //-------------------------------------------------------------------------
    void on_channelsPerPageSpinbox_valueChanged (int value);

    //-------------------------------------------------------------------------
    void on_secsPerPageSpinbox_valueChanged (double value);

    //-------------------------------------------------------------------------
    void updateValues ();

    //-------------------------------------------------------------------------
    void selfUpdatingFinished ();
private:
    //-------------------------------------------------------------------------
    virtual void showEvent (QShowEvent* event);

    SignalVisualisationView const* signal_visualisation_view_;
    QSharedPointer<SignalViewSettings> settings_;
    Ui::AdaptBrowserViewWidget ui_;
    bool self_updating_;
    bool updating_values_;
};

}

#endif // ADAPT_BROWSER_VIEW_WIDGET_H
