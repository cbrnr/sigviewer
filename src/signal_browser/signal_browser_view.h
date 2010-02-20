// signal_browser_view.h
#ifndef SIGNAL_BROWSER_VIEW_H
#define SIGNAL_BROWSER_VIEW_H

#include "../base/user_types.h"

#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSharedPointer>

#include <set>
#include <map>
#include <string>

class QGridLayout;
class QScrollBar;
class QCursor;

namespace BioSig_
{

class LabelWidget;
class YAxisWidget;
class XAxisWidget;
class SignalBrowserModel;
class SignalGraphicsItem;
class EventGraphicsItem;
class EventInfoWidget;

// signal browser view
class SignalBrowserView : public QFrame
{
    Q_OBJECT

public:
    SignalBrowserView (QSharedPointer<SignalBrowserModel> signal_browser_model, QWidget* parent = 0);
    virtual ~SignalBrowserView ();

    void addSignalGraphicsItem (int32 channel_nr, SignalGraphicsItem* graphics_item);
    void removeSignalGraphicsItem (SignalGraphicsItem* graphics_item);
    void addEventGraphicsItem (QSharedPointer<EventGraphicsItem> event_graphics_item);
    void removeEventGraphicsItem (QSharedPointer<EventGraphicsItem> event_graphics_item, bool update_view = true);

    void setScrollMode (bool activated);
    void resizeScene (int32 width, int32 height);
    int32 getVisibleWidth () const;
    int32 getVisibleHeight () const;
    int32 getVisibleX () const;
    int32 getVisibleY () const;

    std::set<std::string> getHideAbleWidgets () const;
    bool getWidgetVisibility (std::string const &widget_name) const;
    void setWidgetVisibility (std::string const &widget_name, bool visibility);
    YAxisWidget& getYAxisWidget () const;
    LabelWidget& getLabelWidget () const;
    XAxisWidget& getXAxisWidget () const;

    void goTo (float32 x, float32 y);
    void smoothGoTo (float32 x, float32 y);
    void setViewCursor (QCursor const &cursor);
    void updateWidgets (bool update_view = true);

signals:
    void visibleXChanged (int32 new_x);
    void visibleYChanged (int32 new_y);

protected:
//    virtual void resizeEvent (QResizeEvent * event);

private slots:
    void verticalSrollbarMoved(int);
    void horizontalSrollbarMoved(int);
    void horizontalScrollBarRangeChaned (int min, int max);
    void verticalScrollBarRangeChaned (int min, int max);
    virtual void dropEvent (QDropEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    void scroll ();

private:
    void createLayout ();

    QTimer* scroll_timer_;
    QGraphicsScene* graphics_scene_;
    QGraphicsView* graphics_view_;

    YAxisWidget* y_axis_widget_;
    XAxisWidget* x_axis_widget_;
    QScrollBar* horizontal_scrollbar_;
    QScrollBar* vertical_scrollbar_;
    LabelWidget* label_widget_;
    EventInfoWidget* event_info_widget_;

    QGridLayout* layout_;
    float32 scroll_x_destination_;
    float32 scroll_x_halfway_;
    int32 scroll_x_step_;
    bool scroll_x_left_;
    std::map<std::string, QWidget*> hideable_widgets_;
};

} // namespace PortingToQT4_

#endif // SIGNAL_BROWSER_VIEW_H

