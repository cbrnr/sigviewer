// signal_browser_view.h
#ifndef SIGNAL_BROWSER_VIEW_H
#define SIGNAL_BROWSER_VIEW_H

#include "../base/user_types.h"
#include "../command_executer.h"
#include "../gui/signal_visualisation_model.h"
#include "../gui/signal_visualisation_view.h"
#include "../file_handling/event_manager.h"

#include "signal_browser_graphics_view.h"

#include <QFrame>
#include <QGraphicsScene>
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
class EventEditingWidget;
class EventCreationWidget;
class AdaptBrowserViewWidget;

// signal browser view
class SignalBrowserView : public QFrame, public SignalVisualisationView
{
    Q_OBJECT

public:
    SignalBrowserView (QSharedPointer<SignalBrowserModel> signal_browser_model,
                       QSharedPointer<EventManager> event_manager,
                       QSharedPointer<CommandExecuter> command_executer,
                       QRect const& initial_size,
                       QWidget* parent = 0);
    virtual ~SignalBrowserView ();

    void addSignalGraphicsItem (ChannelID channel_nr, SignalGraphicsItem* graphics_item, QString const& label);
    void removeSignalGraphicsItem (ChannelID channel_nr, SignalGraphicsItem* graphics_item);
    void addEventGraphicsItem (EventGraphicsItem* event_graphics_item);
    void removeEventGraphicsItem (EventGraphicsItem* event_graphics_item);

    void resizeScene (int32 width, int32 height);
    int32 getVisibleWidth () const;
    int32 getVisibleHeight () const;
    int32 getVisibleX () const;

    void goTo (float32 x);
    void smoothGoTo (float32 x, float32 y);
    void updateWidgets (bool update_view = true);

    virtual QSharedPointer<QImage> renderVisibleScene () const;
    virtual bool getXAxisVisibility () const;
    virtual bool getYAxisVisibility () const;
    virtual bool getLabelsVisibility () const;
public slots:
    void setXAxisIntervall (float64 intervall);
    void setMode (SignalVisualisationMode mode);

signals:
    void visibleXChanged (int32 new_x);
    void visibleYChanged (int32 new_y);

private slots:
    void verticalSrollbarMoved(int);
    void horizontalSrollbarMoved(int);
    void horizontalScrollBarRangeChaned (int min, int max);
    void verticalScrollBarRangeChaned (int min, int max);
    virtual void dropEvent (QDropEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    void scroll ();
    void graphicsSceneResized (QResizeEvent* event);

private:
    void createLayout ();
    void loadSettings ();
    void saveSettings ();

    QSharedPointer<SignalVisualisationModel> model_;

    QTimer* scroll_timer_;
    QGraphicsScene* graphics_scene_;
    SignalBrowserGraphicsView* graphics_view_;

    YAxisWidget* y_axis_widget_;
    XAxisWidget* x_axis_widget_;
    QScrollBar* horizontal_scrollbar_;
    QScrollBar* vertical_scrollbar_;
    LabelWidget* label_widget_;
    EventEditingWidget* event_editing_widget_;
    EventCreationWidget* event_creation_widget_;
    AdaptBrowserViewWidget* adapt_browser_view_widget_;
    QWidget* current_info_widget_;
    QWidget* empty_widget_;

    QGridLayout* layout_;
    float32 scroll_x_destination_;
    float32 scroll_x_halfway_;
    int32 scroll_x_step_;
    bool scroll_x_left_;
    std::map<std::string, QWidget*> hideable_widgets_;
};

} // namespace BioSig_

#endif // SIGNAL_BROWSER_VIEW_H

