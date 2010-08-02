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
