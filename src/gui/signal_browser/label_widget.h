// © SigViewer developers
//
// License: GPL-3.0

#ifndef LABEL_WIDGET_H
#define LABEL_WIDGET_H

#include <QMap>
#include <QSharedPointer>
#include <QWidget>

#include "base/sigviewer_user_types.h"
#include "gui/signal_view_settings.h"

namespace sigviewer {

//-----------------------------------------------------------------------------
class LabelWidget : public QWidget {
    Q_OBJECT
   public:
    LabelWidget(QSharedPointer<SignalViewSettings const> signal_view_settings);

    void addChannel(ChannelID channel_nr, const QString& label);
    void removeChannel(ChannelID channel_nr);

   public slots:
    void changeYStart(int32 y_start);
    void enableSeparator(bool enable);
    void changeLabelColor(QColor labelColor);

   protected:
    void paintEvent(QPaintEvent* pe);
    virtual void contextMenuEvent(QContextMenuEvent* event);
    virtual void changeEvent(QEvent* event);

   private:
    QSharedPointer<SignalViewSettings const> signal_view_settings_;

    QMap<ChannelID, QString> channel_nr2label_;
    int32 y_start_;
    bool enable_separator_;
    QColor label_color_;
};

}  // namespace sigviewer

#endif
