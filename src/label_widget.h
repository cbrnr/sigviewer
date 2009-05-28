// label_widget.h

#ifndef LABEL_WIDGET_H
#define LABEL_WIDGET_H

#include "base/user_types.h"

#include <QMap>
#include <QWidget>
#include <QPaintEvent>

namespace BioSig_
{

#ifndef QT4_PORTED
class SignalBrowserModel;
class SignalBrowser;
#else
namespace PortingToQT4_
{
class SignalBrowserModel;
class SignalBrowserView;
}
#endif

// label widget
class LabelWidget : public QWidget
{
public:
#ifndef QT4_PORTED
    LabelWidget(SignalBrowserModel& model, SignalBrowser* browser);
#else
    LabelWidget(PortingToQT4_::SignalBrowserModel& model, PortingToQT4_::SignalBrowserView* browser);
#endif

    void addChannel(int32 channel_nr, const QString& label);
    void removeChannel(int32 channel_nr);

    static void loadSettings();
    static void saveSettings();

    static const QColor& getAxisColor();

protected:
    void paintEvent(QPaintEvent* pe);

private:
#ifndef QT4_PORTED
    SignalBrowserModel& signal_browser_model_;
    SignalBrowser* signal_browser_;
#else
    PortingToQT4_::SignalBrowserModel& signal_browser_model_;
    PortingToQT4_::SignalBrowserView* signal_browser_;
#endif
    QMap<int32, QString> channel_nr2label_;
    static QColor axis_color_;
};

} // namespace BioSig_

#endif
