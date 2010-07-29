#include "signal_browser_view.h"
#include "signal_browser_model_4.h"
#include "signal_graphics_item.h"
#include "event_graphics_item.h"
#include "y_axis_widget_4.h"
#include "x_axis_widget_4.h"
#include "label_widget.h"
#include "event_creation_widget.h"
#include "event_editing_widget.h"
#include "adapt_browser_view_widget.h"

#include <QGraphicsLineItem>
#include <QGridLayout>
#include <QScrollBar>
#include <QPointF>
#include <QTimer>
#include <QSettings>
#include <QToolBox>
#include <QMenu>
#include <QDebug>
#include <QResizeEvent>
#include <QGraphicsLinearLayout>

#ifdef Q_OS_MACX
#include <QGLWidget>
#endif

namespace BioSig_
{

//-----------------------------------------------------------------------------
// constructor
SignalBrowserView::SignalBrowserView (QSharedPointer<SignalBrowserModel> signal_browser_model,
                                      QSharedPointer<EventManager> event_manager,
                                      QSharedPointer<CommandExecuter> command_executer,
                                      QRect const& initial_size,
                                      QWidget* parent)
: QFrame (parent),
  model_ (signal_browser_model),
  empty_widget_ (new QWidget)
{
    scroll_timer_ = new QTimer (this);
    connect (scroll_timer_, SIGNAL(timeout()), this, SLOT(scroll()));
    resize (initial_size.width(), initial_size.height());
    graphics_scene_ = new QGraphicsScene (0,0,initial_size.width(), initial_size.height(), this);
    graphics_view_ = new SignalBrowserGraphicsView (graphics_scene_);
#ifdef Q_OS_MACX
    graphics_view_->setViewport(new QGLWidget);
#endif
    graphics_view_->setAcceptDrops (false);
    graphics_view_->scroll(0,0);
    graphics_view_->horizontalScrollBar()->hide();
    graphics_view_->verticalScrollBar()->hide();
    graphics_view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphics_view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphics_view_->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    graphics_view_->setDragMode(QGraphicsView::ScrollHandDrag);

    initWidgets (event_manager, command_executer);

    graphics_view_->resize(width() - label_widget_->width() - y_axis_widget_->width() + (vertical_scrollbar_->width()*2), height() - x_axis_widget_->height() + horizontal_scrollbar_->height());

    graphics_view_->setMinimumSize(0, 0);
    graphics_view_->setFrameStyle(QFrame::NoFrame);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    loadSettings();

    createLayout();
    connect (graphics_view_, SIGNAL(resized(QResizeEvent*)), SLOT(graphicsSceneResized(QResizeEvent*)));
}

//-----------------------------------------------------------------------------
SignalBrowserView::~SignalBrowserView ()
{
    saveSettings ();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::resizeScene (int32 width, int32 height)
{
    QPointF left_upper_corner = graphics_view_->mapToScene (0, 0);

    left_upper_corner.setY (left_upper_corner.y() * height / graphics_scene_->height());
    left_upper_corner.setX (left_upper_corner.x() * width / graphics_scene_->width());

    graphics_scene_->setSceneRect (0, 0, width, height);
    QPointF center = left_upper_corner;
    center.setX (center.x () + static_cast<double>(graphics_view_->width()) / 2.0);
    center.setY (center.y () + static_cast<double>(graphics_view_->height()) / 2.0);
    graphics_view_->centerOn (center);

//    y_axis_widget_->changeYStart (left_upper_corner.y());
    emit visibleXChanged (graphics_view_->mapToScene (0, 0).x());
    emit visibleYChanged (graphics_view_->mapToScene(0, 0).y());
}

//-----------------------------------------------------------------------------
void SignalBrowserView::addSignalGraphicsItem (ChannelID channel_nr, SignalGraphicsItem* graphics_item, QString const& label)
{
    graphics_scene_->addItem (graphics_item);
    y_axis_widget_->addChannel (channel_nr, graphics_item);
    label_widget_->addChannel (channel_nr, label);

    connect (graphics_item, SIGNAL(updatedYGrid(ChannelID)), y_axis_widget_, SLOT(updateChannel(ChannelID)));
    connect (graphics_item, SIGNAL(shifting(ChannelID)), y_axis_widget_, SLOT(updateChannel(ChannelID)));
    connect (graphics_item, SIGNAL(mouseAtSecond(float64)), x_axis_widget_, SLOT(changeHighlightTime(float64)));
    connect (graphics_item, SIGNAL(mouseMoving(bool)), x_axis_widget_, SLOT(enableHighlightTime(bool)));
}

//-----------------------------------------------------------------------------
void SignalBrowserView::removeSignalGraphicsItem (ChannelID channel_nr, SignalGraphicsItem* graphics_item)
{
    disconnect (graphics_item, 0, x_axis_widget_, 0);
    disconnect (graphics_item, 0, y_axis_widget_, 0);

    y_axis_widget_->removeChannel(channel_nr);
    label_widget_->removeChannel(channel_nr);
    graphics_scene_->removeItem (graphics_item);
    graphics_view_->update ();
}


//-----------------------------------------------------------------------------
void SignalBrowserView::addEventGraphicsItem (EventGraphicsItem* event_graphics_item)
{
    graphics_scene_->addItem(event_graphics_item);
    graphics_view_->update();
    connect (event_graphics_item, SIGNAL(mouseAtSecond(float64)), x_axis_widget_, SLOT(changeHighlightTime(float64)));
    connect (event_graphics_item, SIGNAL(mouseMoving(bool)), x_axis_widget_, SLOT(enableHighlightTime(bool)));
}

//-----------------------------------------------------------------------------
void SignalBrowserView::removeEventGraphicsItem (EventGraphicsItem* event_graphics_item)
{
    qDebug () << "SignalBrowserView::removeEventGraphicsItem " << event_graphics_item->getId();
    graphics_view_->update();
    qDebug () << "SignalBrowserView::removeEventGraphicsItem " << event_graphics_item->getId() << " finished";
}


//-----------------------------------------------------------------------------
int32 SignalBrowserView::getVisibleWidth () const
{
    return graphics_view_->viewport()->width();
}

//-----------------------------------------------------------------------------
int32 SignalBrowserView::getVisibleHeight () const
{
    return graphics_view_->viewport()->height();
}

//-----------------------------------------------------------------------------
int32 SignalBrowserView::getVisibleX () const
{
    return graphics_view_->mapToScene(0,0).x();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::goTo (float32 x)
{
    double y = graphics_view_->mapToScene(0, 0).y();
    x += graphics_view_->width() / 2;
    y += static_cast<double>(graphics_view_->height()) / 2.0;
    graphics_view_->centerOn(x, y);
    graphics_scene_->update(0, 0, graphics_scene_->width(), graphics_scene_->height());
}

//-----------------------------------------------------------------------------
void SignalBrowserView::smoothGoTo (float32 x, float32)
{
    scroll_timer_->setInterval (1);
    scroll_x_halfway_ = x + graphics_view_->mapToScene(0,0).x();
    scroll_x_halfway_ /= 2;
    scroll_x_step_ = 10;
    if (scroll_x_halfway_ > x)
        scroll_x_left_ = true;
    else
        scroll_x_left_ = false;
    scroll_x_destination_ = x;
    scroll_timer_->start();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::updateWidgets (bool update_view)
{
    if (update_view)
        graphics_view_->viewport()->update();
    x_axis_widget_->update();
    label_widget_->update();
    emit visibleYChanged (graphics_view_->mapToScene(0,0).y());
}

//-----------------------------------------------------------------------------
QSharedPointer<QImage> SignalBrowserView::renderVisibleScene () const
{
    QSharedPointer<QImage> image (new QImage(graphics_view_->viewport()->width(),
                                             graphics_view_->viewport()->height(),
                                             QImage::Format_ARGB32));
    image->fill (0);
    QPainter painter (image.data());
    graphics_view_->render (&painter, graphics_view_->viewport()->rect(), graphics_view_->viewport()->rect());
    return image;
}

//-----------------------------------------------------------------------------
bool SignalBrowserView::getXAxisVisibility () const
{
    return x_axis_widget_->isVisible ();
}

//-----------------------------------------------------------------------------
bool SignalBrowserView::getYAxisVisibility () const
{
    return y_axis_widget_->isVisible ();
}

//-----------------------------------------------------------------------------
bool SignalBrowserView::getLabelsVisibility () const
{
    return label_widget_->isVisible ();
}

//-----------------------------------------------------------------------------
int SignalBrowserView::getViewportHeight () const
{
    return graphics_view_->viewport()->height();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::setMode (SignalVisualisationMode mode)
{
    if (current_info_widget_)
    {
        layout_->removeWidget (current_info_widget_);
        current_info_widget_->hide();
    }
    switch (mode)
    {
    case MODE_NEW:
        current_info_widget_ = event_creation_widget_;
        graphics_view_->setDragMode(QGraphicsView::NoDrag);
        break;
    case MODE_POINTER:
        current_info_widget_ = event_editing_widget_;
        graphics_view_->setDragMode(QGraphicsView::NoDrag);
        break;
    case MODE_HAND:
        graphics_view_->setDragMode(QGraphicsView::ScrollHandDrag);
        current_info_widget_ = empty_widget_;
        break;
    case MODE_VIEW_OPTIONS:
        current_info_widget_ = adapt_browser_view_widget_;
        graphics_view_->setDragMode(QGraphicsView::NoDrag);
        break;
    case MODE_INFO:
        current_info_widget_ = model_->infoWidget ();
        graphics_view_->setDragMode(QGraphicsView::NoDrag);
        break;
    default:
        current_info_widget_ = empty_widget_;
        break;
    }
    if (current_info_widget_)
    {
        layout_->addWidget (current_info_widget_, 1, 2);
        current_info_widget_->show();
    }
}

//-----------------------------------------------------------------------------
void SignalBrowserView::graphicsSceneResized (QResizeEvent* event)
{
    unsigned channel_height = model_->getSignalViewSettings()->getChannelHeight();
    if (!channel_height)
        return;

    if (event->size().height() < 1)
        return;

    if (event->oldSize().height() == event->size().height())
        return;

    double channels_per_pagesize = event->oldSize().height() / channel_height;

    channel_height = event->size().height() / channels_per_pagesize;
    model_->getSignalViewSettings()->setChannelHeight (channel_height);
}

//-----------------------------------------------------------------------------
void SignalBrowserView::verticalSrollbarMoved(int)
{
    label_widget_->update();
    qreal y = graphics_view_->mapToScene(0,0).y();
    if (y < 0)
        y = 0;
    emit visibleYChanged (y);
}

//-----------------------------------------------------------------------------
void SignalBrowserView::horizontalSrollbarMoved(int)
{
    emit visibleXChanged (graphics_view_->mapToScene(0,0).x());
}

//-----------------------------------------------------------------------------
void SignalBrowserView::horizontalScrollBarRangeChaned (int min, int max)
{
    horizontal_scrollbar_->setRange(min, max);
    horizontal_scrollbar_->setPageStep(graphics_view_->horizontalScrollBar()->pageStep());
    emit visibleXChanged (graphics_view_->mapToScene(0,0).x());
}

//-----------------------------------------------------------------------------
void SignalBrowserView::verticalScrollBarRangeChaned (int min, int max)
{
    label_widget_->update ();
    vertical_scrollbar_->setRange(min, max);
    vertical_scrollbar_->setPageStep(graphics_view_->verticalScrollBar()->pageStep());
    qreal y = graphics_view_->mapToScene(0,0).y();
    if (y < 0)
        y = 0;
    emit visibleYChanged (y);
}

//-----------------------------------------------------------------------------
void SignalBrowserView::dropEvent (QDropEvent* event)
{
    event->ignore();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::dragEnterEvent(QDragEnterEvent *event)
{
    event->ignore();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::scroll ()
{
    if ((scroll_x_left_ && graphics_view_->mapToScene(0,0).x() >= scroll_x_halfway_)
        || (!scroll_x_left_ && graphics_view_->mapToScene(0,0).x() <= scroll_x_halfway_))
        scroll_x_step_ += 10;
    else
        scroll_x_step_ -= 10;

    if (scroll_x_step_ < 0)
        scroll_x_step_ = 1;

    horizontal_scrollbar_->setValue (horizontal_scrollbar_->value() + (scroll_x_left_ ? -scroll_x_step_ : scroll_x_step_));
    if ((scroll_x_left_ && graphics_view_->mapToScene(0,0).x() <= scroll_x_destination_)
        || (!scroll_x_left_ && graphics_view_->mapToScene(0,0).x() >= scroll_x_destination_))
        scroll_timer_->stop();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::initWidgets (QSharedPointer<EventManager> event_manager, QSharedPointer<CommandExecuter> command_executer)
{
    y_axis_widget_ = new YAxisWidget (this);
    hideable_widgets_["Y Axis"] = y_axis_widget_;
    y_axis_widget_->resize(70, height());
    y_axis_widget_->setMinimumSize(70, 0);

    x_axis_widget_ = new XAxisWidget (model_->getSignalViewSettings(), this);
    hideable_widgets_["X Axis"] = x_axis_widget_;
    x_axis_widget_->resize(width()-300, 30);
    x_axis_widget_->setMinimumSize(0, 30);

    horizontal_scrollbar_ = new QScrollBar (Qt::Horizontal, this);
    vertical_scrollbar_ = new QScrollBar (Qt::Vertical, this);

    label_widget_ = new LabelWidget (*model_);
    hideable_widgets_["Channel Labels"] = label_widget_;

    current_info_widget_ = empty_widget_;
    if (event_manager.isNull())
    {
        event_creation_widget_ = 0;
        event_editing_widget_ = 0;
    }
    else
    {
        event_creation_widget_ = new EventCreationWidget (model_, event_manager);
        event_editing_widget_ = new EventEditingWidget (event_manager, command_executer);
        event_creation_widget_->connect (model_.data(), SIGNAL(shownEventTypesChanged(std::set<EventType> const&)), SLOT(updateShownEventTypes (std::set<EventType> const&)));
        event_editing_widget_->connect (model_.data(), SIGNAL(shownEventTypesChanged(std::set<EventType> const&)), SLOT(updateShownEventTypes (std::set<EventType> const&)));
        event_editing_widget_->connect (model_.data(), SIGNAL(eventSelected(QSharedPointer<SignalEvent const>)), SLOT(updateSelectedEventInfo(QSharedPointer<SignalEvent const>)));
    }

    adapt_browser_view_widget_ = new AdaptBrowserViewWidget (this, model_->getSignalViewSettings());
    x_axis_widget_->connect (adapt_browser_view_widget_, SIGNAL(xAxisVisibilityChanged(bool)), SLOT(setVisible(bool)));
    y_axis_widget_->connect (adapt_browser_view_widget_, SIGNAL(yAxisVisibilityChanged(bool)), SLOT(setVisible(bool)));
    label_widget_->connect (adapt_browser_view_widget_, SIGNAL(labelsVisibilityChanged(bool)), SLOT(setVisible(bool)));

    connect(horizontal_scrollbar_, SIGNAL(valueChanged(int)),
            graphics_view_->horizontalScrollBar(), SLOT(setValue(int)));
    connect(graphics_view_->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            horizontal_scrollbar_, SLOT(setValue(int)));
    connect(graphics_view_->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)),
            this, SLOT(horizontalScrollBarRangeChaned(int,int)));
    connect(horizontal_scrollbar_, SIGNAL(valueChanged(int)),
            this, SLOT(horizontalSrollbarMoved(int)));

    connect(vertical_scrollbar_, SIGNAL(valueChanged(int)),
            graphics_view_->verticalScrollBar(), SLOT(setValue(int)));
    connect(graphics_view_->verticalScrollBar(), SIGNAL(valueChanged(int)),
            vertical_scrollbar_, SLOT(setValue(int)));
    connect(graphics_view_->verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
            this, SLOT(verticalScrollBarRangeChaned(int,int)));
    connect(vertical_scrollbar_, SIGNAL(valueChanged(int)),
            this, SLOT(verticalSrollbarMoved(int)));

    connect(this, SIGNAL(visibleXChanged(int32)), x_axis_widget_, SLOT(changeXStart(int32)));
    connect(model_->getSignalViewSettings().data(), SIGNAL(pixelsPerSampleChanged()), x_axis_widget_, SLOT(update()));
    label_widget_->connect (this, SIGNAL(visibleYChanged(int32)), SLOT(changeYStart (int32)));
    connect(this, SIGNAL(visibleYChanged(int32)), y_axis_widget_, SLOT(changeYStart(int32)));
    connect(model_->getSignalViewSettings().data(), SIGNAL(channelHeightChanged(uint)), y_axis_widget_, SLOT(changeSignalHeight(uint)));
    connect(model_.data(), SIGNAL(modeChanged(SignalVisualisationMode)), SLOT(setMode(SignalVisualisationMode)));
}

//-----------------------------------------------------------------------------
void SignalBrowserView::createLayout()
{
    layout_ = new QGridLayout(this);

    layout_->setMargin(0);
    layout_->setSpacing(0);
    layout_->setVerticalSpacing(0);
    layout_->setHorizontalSpacing(0);

    layout_->addWidget(current_info_widget_, 1, 1, 1, 3);
    layout_->addWidget(y_axis_widget_, 2, 1);
    layout_->addWidget(graphics_view_, 2, 2);
    layout_->addWidget(x_axis_widget_, 3, 2);
    layout_->addWidget(horizontal_scrollbar_, 4, 2);
    layout_->addWidget(label_widget_, 2, 3);
    layout_->addWidget(vertical_scrollbar_, 2, 4);
}

//-----------------------------------------------------------------------------
void SignalBrowserView::loadSettings ()
{
    QSettings settings("SigViewer");
    settings.beginGroup("SignalBrowserView");

    for (std::map<std::string, QWidget*>::iterator widget_iterator =
         hideable_widgets_.begin();
         widget_iterator != hideable_widgets_.end();
         ++widget_iterator)
    {
        widget_iterator->second->setVisible (settings.value(widget_iterator->first.c_str()).toBool());
    }

    settings.endGroup();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::saveSettings ()
{
    QSettings settings("SigViewer");
    settings.beginGroup("SignalBrowserView");

    for (std::map<std::string, QWidget*>::const_iterator widget_iterator =
         hideable_widgets_.begin();
         widget_iterator != hideable_widgets_.end();
         ++widget_iterator)
    {
        settings.setValue(widget_iterator->first.c_str(),
                          widget_iterator->second->isVisibleTo (this));
    }

    settings.endGroup();
}

} // namespace BioSig_
