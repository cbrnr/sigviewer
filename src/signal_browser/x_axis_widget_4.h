#ifndef X_AXIS_WIDGET_H
#define X_AXIS_WIDGET_H

#include <QWidget>

namespace BioSig_
{

class SignalBrowserModel;
class SignalBrowserView;


class XAxisWidget : public QWidget
{
public:
    XAxisWidget(QWidget* parent,
                SignalBrowserModel& model, SignalBrowserView* browser);
    virtual ~XAxisWidget();
    virtual QSize sizeHint () const;

private:
    virtual void paintEvent(QPaintEvent* event = 0);

    SignalBrowserModel& signal_browser_model_;
    SignalBrowserView* signal_browser_;
};

}

#endif // X_AXIS_WIDGET_H
