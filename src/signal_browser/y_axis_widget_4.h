#ifndef Y_AXIS_GRAPHICS_ITEM_H
#define Y_AXIS_GRAPHICS_ITEM_H

#include "../base/user_types.h"

#include <QWidget>
#include <QMap>

namespace BioSig_
{

namespace PortingToQT4_
{

class SignalBrowserModel;
class SignalBrowserView;
class SignalGraphicsItem;

class YAxisWidget : public QWidget
{
public:
    YAxisWidget(QWidget* parent,
                SignalBrowserModel& model, SignalBrowserView* browser);

    virtual QSize sizeHint () const;

    void addChannel(int32 channel_nr, SignalGraphicsItem* signal_item);
    void removeChannel(int32 channel_nr);

private:
    virtual void paintEvent(QPaintEvent* event = 0);

    SignalBrowserModel& signal_browser_model_;
    SignalBrowserView* signal_browser_;

    QMap<int32, SignalGraphicsItem*> channel_nr2signal_graphics_item_;
};

}

}

#endif // Y_AXIS_GRAPHICS_ITEM_H
