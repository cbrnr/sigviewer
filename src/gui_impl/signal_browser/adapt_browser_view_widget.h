// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


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
    ~AdaptBrowserViewWidget();

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

    void on_xGridCheckbox_stateChanged(int checkState);
    void on_yGridCheckbox_stateChanged(int checkState);

    void on_offsetCheckBox_stateChanged(int checkState);

    void on_checkBox_stateChanged(int checkState);

private:
    //-------------------------------------------------------------------------
    virtual void showEvent (QShowEvent* event);

    SignalVisualisationView const* signal_visualisation_view_;
    QSharedPointer<SignalViewSettings> settings_;
    Ui::AdaptBrowserViewWidget ui_;
    bool self_updating_;
    bool updating_values_;
    QAction* offset_centered_;
    QAction* offset_fitted_;
};

}

#endif // ADAPT_BROWSER_VIEW_WIDGET_H
