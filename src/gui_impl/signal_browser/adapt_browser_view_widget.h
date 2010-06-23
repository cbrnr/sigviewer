#ifndef ADAPT_BROWSER_VIEW_WIDGET_H
#define ADAPT_BROWSER_VIEW_WIDGET_H

#include "../../gui/signal_visualisation_view.h"
#include "ui_adapt_browser_view_widget.h"

#include <QWidget>

namespace BioSig_
{

//-------------------------------------------------------------------------
class AdaptBrowserViewWidget : public QWidget
{
Q_OBJECT
public:
    //-------------------------------------------------------------------------
    explicit AdaptBrowserViewWidget (SignalVisualisationView const* signal_visualisation_view,
                                     QWidget *parent = 0);
signals:
    //-------------------------------------------------------------------------
    void xAxisVisibilityChanged (bool visible);

    //-------------------------------------------------------------------------
    void yAxisVisibilityChanged (bool visible);

    //-------------------------------------------------------------------------
    void labelsVisibilityChanged (bool visible);

private:
    //-------------------------------------------------------------------------
    virtual void showEvent (QShowEvent* event);

    SignalVisualisationView const* signal_visualisation_view_;
    Ui::AdaptBrowserViewWidget ui_;
};

}

#endif // ADAPT_BROWSER_VIEW_WIDGET_H
