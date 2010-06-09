#ifndef LABEL_WIDGET_H
#define LABEL_WIDGET_H

#include "../base/user_types.h"
#include "signal_browser_model_4.h"

#include <QMap>
#include <QWidget>

namespace BioSig_
{

//-----------------------------------------------------------------------------
class LabelWidget : public QWidget
{
    Q_OBJECT
public:
    LabelWidget(SignalBrowserModel& model);

    void addChannel(ChannelID channel_nr, const QString& label);
    void removeChannel(ChannelID channel_nr);

public slots:
    void changeYStart (int32 y_start);

protected:
    void paintEvent(QPaintEvent* pe);
    virtual void contextMenuEvent (QContextMenuEvent* event);

private:
    SignalBrowserModel& signal_browser_model_;

    unsigned signal_height_;

    QMap<ChannelID, QString> channel_nr2label_;
    int32 y_start_;
};

} // namespace BioSig_

#endif
