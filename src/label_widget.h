// label_widget.h

#ifndef LABEL_WIDGET_H
#define LABEL_WIDGET_H

#include "base/user_types.h"

#include <QMap>
#include <QWidget>
#include <QPaintEvent>

namespace BioSig_
{

class SignalBrowserModel;
class SignalBrowser;

// label widget
class LabelWidget : public QWidget
{
public:
    LabelWidget(SignalBrowserModel& model, SignalBrowser* browser);

    void addChannel(int32 channel_nr, const QString& label);
    void removeChannel(int32 channel_nr);

    static void loadSettings();
    static void saveSettings();

    static const QColor& getAxisColor();

protected:
    void paintEvent(QPaintEvent* pe);

private:
    SignalBrowserModel& signal_browser_model_;
    SignalBrowser* signal_browser_;
    QMap<int32, QString> channel_nr2label_;
    static QColor axis_color_;
};

} // namespace BioSig_

#endif
