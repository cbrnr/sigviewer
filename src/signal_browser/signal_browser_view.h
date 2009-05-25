// signal_browser_view.h
#ifndef SIGNAL_BROWSER_VIEW_H
#define SIGNAL_BROWSER_VIEW_H

#include "../base/user_types.h"

#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>

class QGridLayout;
class QScrollBar;

namespace BioSig_
{

namespace PortingToQT4_
{

class YAxisWidget;
class XAxisWidget;
class SignalBrowserModel;
class SignalGraphicsItem;

// signal browser view
class SignalBrowserView : public QFrame
{
    Q_OBJECT

public:
    SignalBrowserView(SignalBrowserModel* signal_browser_model, QWidget* parent = 0);
    virtual ~SignalBrowserView ();

    void addSignalGraphicsItem (int32 channel_nr, SignalGraphicsItem* graphics_item);
    void removeSignalGraphicsItem (int32 channel_nr);


    void resizeScene (int32 width, int32 height);
    int32 getVisibleWidth () const;
    int32 getVisibleHeight () const;
    int32 getVisibleX () const;
    int32 getVisibleY () const;
    YAxisWidget& getYAxisWidget () const;

    void scrollContente (int32 dx, int32 dy);
    void updateWidgets ();
    //-------------------------------------------------------------------------
    /// work-around function... :(
    void initScroll ();

protected:
    virtual void resizeEvent (QResizeEvent * event);

private slots:
    void verticalSrollbarMoved(int value);
    void horizontalSrollbarMoved(int value);
    void horizontalScrollBarRangeChaned (int min, int max);
    void verticalScrollBarRangeChaned (int min, int max);

private:
    void createLayout ();

    QGraphicsScene* graphics_scene_;
    QGraphicsView* graphics_view_;

    int32 center_x_for_scrolling_;
    int32 center_y_for_scrolling_;

    YAxisWidget* y_axis_widget_;
    XAxisWidget* x_axis_widget_;
    QScrollBar* horizontal_scrollbar_;
    QScrollBar* vertical_scrollbar_;

    QGridLayout* layout_;


};

} // namespace PortingToQT4_

} // namespace BioSig_

#endif // SIGNAL_BROWSER_VIEW_H

