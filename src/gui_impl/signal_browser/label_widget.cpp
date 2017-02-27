// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "label_widget.h"
#include "gui/gui_action_factory.h"

#include <cmath>

#include <QPainter>
#include <QContextMenuEvent>

namespace sigviewer
{

//-----------------------------------------------------------------------------
LabelWidget::LabelWidget (QSharedPointer<SignalViewSettings const> signal_view_settings)
: signal_view_settings_ (signal_view_settings),
  y_start_ (0),
  enable_borderline_ (true),
  label_color_ (Qt::black)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void LabelWidget::changeYStart (int32 y_start)
{
    y_start_ = y_start;
    update ();
}

//-----------------------------------------------------------------------------
void LabelWidget::enableBorderline(bool enable)
{
    enable_borderline_ = enable;
    update();
}

void LabelWidget::changeLabelColor(QColor labelColor)
{
    label_color_ = labelColor;
    update();
}


//-----------------------------------------------------------------------------
void LabelWidget::paintEvent(QPaintEvent*)
{
    bool channel_overlapping = signal_view_settings_->getChannelOverlapping();
    float64 signal_height = signal_view_settings_->getChannelHeight();
    if (channel_overlapping)
        signal_height =
                (signal_height +
                 (signal_height * (channel_nr2label_.size() - 1)
                  * (1.0 - signal_view_settings_->getChannelOverlapping())))
                / channel_nr2label_.size();
    int32 y_end = y_start_ + height();

    if (signal_height < 1)
        return;

    QPainter painter(this);
    painter.translate(0, -y_start_);
    painter.setPen(label_color_);
//    painter.setPen(QColor(0, 43, 130));
//    painter.drawLine(0, y_start_, 0, y_end);
//    painter.setPen(Qt::black);

    // labels
    float64 float_y_start = y_start_;//floor(static_cast<float64>(y_start) / intervall) * intervall;
    float64 float_y_end = y_end;//ceil(y_end / intervall) * intervall;
    QMap<ChannelID, QString>::iterator iter = channel_nr2label_.begin();

    for (float32 float_y = signal_height / 2;
         float_y < float_y_end && iter != channel_nr2label_.end();
         float_y += signal_height, iter++)
    {
        if (float_y > float_y_start)
        {
            int32 y = (int32)(float_y + 0.5);
            painter.drawText(5, (int32)(y - signal_height /2) ,
                             width() - 10, (int32)signal_height,
                             Qt::AlignHCenter | Qt::AlignVCenter, iter.value());
        }
    }

    if (channel_overlapping)
        return;
    if (enable_borderline_)
    {
        for (float32 float_y = 0;
             float_y <= signal_height * channel_nr2label_.size();
             float_y += signal_height)
        {
            painter.drawLine(0, float_y, width() - 1, float_y);
        }
    }
}

//-----------------------------------------------------------------------------
void LabelWidget::contextMenuEvent (QContextMenuEvent* event)
{
    QMenu menu;
    menu.addAction(GuiActionFactory::getInstance()->getQAction("Channels per Page..."));
    menu.addSeparator();
    QAction* visibility_action = menu.addAction("Labels");
    visibility_action->setCheckable (true);
    visibility_action->setChecked (true);
    connect (visibility_action, SIGNAL(triggered(bool)), SLOT(setVisible(bool)));
    menu.exec (event->globalPos());
}

//-----------------------------------------------------------------------------
void LabelWidget::addChannel(ChannelID channel_nr, const QString& label)
{
    channel_nr2label_[channel_nr] = label;
    QPixmap dummy(1,1);
    QPainter painter(&dummy);
    QRect bounding = painter.boundingRect(0, 0, 500, 500,
                                    Qt::AlignHCenter | Qt::AlignVCenter, label);
    int32 max_width = minimumWidth() - 10;
    max_width = bounding.width() > max_width ? bounding.width() : max_width;
    setMinimumWidth(10 + max_width);
}

//-----------------------------------------------------------------------------
void LabelWidget::removeChannel(ChannelID channel_nr)
{
    channel_nr2label_.erase(channel_nr2label_.find(channel_nr));
    int32 max_width = -1;
    QPixmap dummy(1,1);
    QPainter p(&dummy);

    for (QMap<ChannelID, QString>::iterator iter = channel_nr2label_.begin();
         iter != channel_nr2label_.end();
         iter++)
    {
        QRect bounding = p.boundingRect(0, 0, 200, p.font().pixelSize(),
                                        Qt::AlignHCenter | Qt::AlignVCenter,
                                        iter.value());

        max_width = max_width < bounding.width() ? bounding.width() : max_width;
    }

    setMinimumWidth(10 + max_width);
}

}

