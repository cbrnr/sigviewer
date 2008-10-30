// x_axis_widget.h

#ifndef X_AXIS_WIDGET_H
#define X_AXIS_WIDGET_H

#include "base/user_types.h"

#include <QWidget>
#include <QPaintEvent>

namespace BioSig_
{

class SignalBrowserModel;
class SignalBrowser;

// x axis widget
class XAxisWidget : public QWidget
{

public:

    XAxisWidget(SignalBrowserModel& model, SignalBrowser* browser);

    static void loadSettings();
    static void saveSettings();
    static const QColor& getAxisColor();

protected:

    void paintEvent(QPaintEvent* pe);

private:

    SignalBrowserModel& signal_browser_model_;
    SignalBrowser* signal_browser_;
    static QColor axis_color_;
};

} // namespace BioSig_

#endif

