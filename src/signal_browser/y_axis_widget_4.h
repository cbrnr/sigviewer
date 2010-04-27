#ifndef Y_AXIS_GRAPHICS_ITEM_H
#define Y_AXIS_GRAPHICS_ITEM_H

#include "../base/user_types.h"

#include <QWidget>
#include <QMap>

class QPixmap;

namespace BioSig_
{

class SignalGraphicsItem;

class YAxisWidget : public QWidget
{
    Q_OBJECT
public:
    YAxisWidget(QWidget* parent);//, SignalBrowserModel& model, SignalBrowserView* browser);

    virtual ~YAxisWidget ();
    virtual QSize sizeHint () const;

    void addChannel(int32 channel_nr, SignalGraphicsItem const* const signal_item);
    void removeChannel(int32 channel_nr);

public slots:
    void changeSignalHeight (uint32 signal_height);
    void changeSignalSpacing (unsigned signal_spacing);
    void changeYStart (int32 y_start);
    void updateChannel (ChannelID channel_nr);
    void updateAllChannels ();

private:
    virtual void paintEvent(QPaintEvent* event = 0);
    void repaintPixmap (int32 channel = -1);

    QPixmap* pixmap_;
    unsigned signal_height_;
    unsigned signal_spacing_;
    int32 pixmap_drawing_height_;
    int32 y_start_;

    QMap<int32, SignalGraphicsItem const*> channel_nr2signal_graphics_item_;
};

}

#endif // Y_AXIS_GRAPHICS_ITEM_H
