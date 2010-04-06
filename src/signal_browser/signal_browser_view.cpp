// signal_browser.cpp

#include "signal_browser_view.h"
#include "signal_browser_model_4.h"
#include "signal_graphics_item.h"
#include "event_graphics_item.h"
#include "y_axis_widget_4.h"
#include "x_axis_widget_4.h"
#include "../label_widget.h"
#include "event_info_widget.h"

#include <QGraphicsLineItem>
#include <QGridLayout>
#include <QScrollBar>
#include <QPointF>
#include <QTimer>
#include <QSettings>
#include <QToolBox>
#include <QMenu>

namespace BioSig_
{

//-----------------------------------------------------------------------------
// constructor
SignalBrowserView::SignalBrowserView (QSharedPointer<SignalBrowserModel> signal_browser_model,
                                      EventManager& event_manager,
                                      CommandExecuter& command_executer,
                                      QWidget* parent)
: QFrame(parent)
{
    scroll_timer_ = new QTimer (this);
    connect (scroll_timer_, SIGNAL(timeout()), this, SLOT(scroll()));
    resize(parent->contentsRect().width(), parent->contentsRect().height());
    graphics_scene_ = new QGraphicsScene (0,0,parent->contentsRect().width(), parent->contentsRect().height(), this);
    graphics_view_ = new QGraphicsView(graphics_scene_, this);
    graphics_view_->setAcceptDrops (false);
    graphics_view_->scroll(0,0);
    graphics_view_->horizontalScrollBar()->hide();
    graphics_view_->verticalScrollBar()->hide();
    graphics_view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphics_view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphics_view_->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    y_axis_widget_ = new YAxisWidget (this);//, *signal_browser_model, this);
    hideable_widgets_["Y Axis"] = y_axis_widget_;
    y_axis_widget_->resize(70, height());
    y_axis_widget_->setMinimumSize(70, 0);

    x_axis_widget_ = new XAxisWidget (this);
    hideable_widgets_["X Axis"] = x_axis_widget_;
    x_axis_widget_->resize(width()-300, 30);
    x_axis_widget_->setMinimumSize(0, 30);

    horizontal_scrollbar_ = new QScrollBar (Qt::Horizontal, this);
    vertical_scrollbar_ = new QScrollBar (Qt::Vertical, this);

    label_widget_ = new LabelWidget (*signal_browser_model, this);
    hideable_widgets_["Channel Labels"] = label_widget_;

    event_info_widget_ = new EventInfoWidget (this, event_manager,
                                              command_executer, signal_browser_model);
    hideable_widgets_["Event Toolbar"] = event_info_widget_;

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
    connect(this, SIGNAL(visibleYChanged(int32)), y_axis_widget_, SLOT(changeYStart(int32)));
    connect(signal_browser_model.data(), SIGNAL(signalHeightChanged(unsigned)), y_axis_widget_, SLOT(changeSignalHeight(unsigned)));
    connect(event_info_widget_, SIGNAL(eventCreationTypeChanged(uint16)), signal_browser_model.data(), SLOT(setActualEventCreationType(uint16)));
    connect(signal_browser_model.data(), SIGNAL(eventSelected(QSharedPointer<SignalEvent const>)), event_info_widget_, SLOT(updateSelectedEventInfo(QSharedPointer<SignalEvent const>)));
    connect(signal_browser_model.data(), SIGNAL(shownEventTypesChanged(std::set<uint16>)), event_info_widget_, SLOT(updateShownEventTypes(std::set<uint16>)));

    graphics_view_->resize(width() - label_widget_->width() - y_axis_widget_->width() + (vertical_scrollbar_->width()*2), height() - x_axis_widget_->height() + horizontal_scrollbar_->height());

    // graphics_view_->setVerticalScrollBarPolicy(setViewportMargins(0, 0, 0, 0);
    //graphics_view_->setOptimizationFlag(QGraphicsView::DontClipPainter, true);
    graphics_view_->setMinimumSize(0, 0);
    graphics_view_->setFrameStyle(QFrame::NoFrame);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    loadSettings();

    createLayout();
}

//-----------------------------------------------------------------------------
SignalBrowserView::~SignalBrowserView ()
{
    saveSettings ();
//    QList<QGraphicsItem*> items = graphics_scene_->items();
//    for (QList<QGraphicsItem*>::iterator it = items.begin();
//         it != items.end();
//         ++it)
//        graphics_scene_->removeItem(*it);
}

//-----------------------------------------------------------------------------
void SignalBrowserView::setScrollMode (bool activated)
{
    if (activated)
        graphics_view_->setDragMode(QGraphicsView::ScrollHandDrag);
    else
        graphics_view_->setDragMode(QGraphicsView::NoDrag);
}


//-----------------------------------------------------------------------------
void SignalBrowserView::resizeScene (int32 width, int32 height)
{
    graphics_scene_->setSceneRect(0, 0, width, height);
    graphics_view_->centerOn(0,0);
    //graphics_view_->viewport()->set
}

//-----------------------------------------------------------------------------
void SignalBrowserView::addSignalGraphicsItem (int32 channel_nr, SignalGraphicsItem* graphics_item)
{
    // TODO: really remove before add????
    //graphics_scene_->removeItem(graphics_item);
    graphics_scene_->addItem(graphics_item);
    y_axis_widget_->addChannel(channel_nr, graphics_item);
    label_widget_->addChannel(channel_nr, graphics_item->getLabel());

    graphics_view_->update();

    connect (graphics_item, SIGNAL(mouseAtSecond(float64)), x_axis_widget_, SLOT(changeHighlightTime(float64)));
    connect (graphics_item, SIGNAL(mouseMoving(bool)), x_axis_widget_, SLOT(enableHighlightTime(bool)));
}

//-----------------------------------------------------------------------------
void SignalBrowserView::removeSignalGraphicsItem (int32 channel_nr, SignalGraphicsItem* graphics_item)
{
    y_axis_widget_->removeChannel(channel_nr);
    label_widget_->removeChannel(channel_nr);
    graphics_scene_->removeItem (graphics_item);
    graphics_view_->update ();
}


//-----------------------------------------------------------------------------
void SignalBrowserView::addEventGraphicsItem (EventGraphicsItem* event_graphics_item)
{
    // TODO: really remove before add????
    graphics_scene_->removeItem(event_graphics_item);
    graphics_scene_->addItem(event_graphics_item);

    graphics_view_->update();
    connect (event_graphics_item, SIGNAL(hoverEnterSignalEvent (QSharedPointer<SignalEvent const>)), event_info_widget_, SLOT(addHoveredEvent(QSharedPointer<SignalEvent const>)));
    connect (event_graphics_item, SIGNAL(hoverLeaveSignalEvent(QSharedPointer<SignalEvent const>)), event_info_widget_, SLOT(removeHoveredEvent(QSharedPointer<SignalEvent const>)));
    connect (event_graphics_item, SIGNAL(mouseAtSecond(float64)), x_axis_widget_, SLOT(changeHighlightTime(float64)));
    connect (event_graphics_item, SIGNAL(mouseMoving(bool)), x_axis_widget_, SLOT(enableHighlightTime(bool)));
}

//-----------------------------------------------------------------------------
void SignalBrowserView::removeEventGraphicsItem (EventGraphicsItem* event_graphics_item,
                                                 bool update_view)
{
    graphics_scene_->removeItem(event_graphics_item);
    if (update_view)
        graphics_view_->update();
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
int32 SignalBrowserView::getVisibleY () const
{
    return graphics_view_->mapToScene(0,0).y();
}

//-----------------------------------------------------------------------------
std::map<std::string, bool> SignalBrowserView::getWidgetVisibilities () const
{
    std::map<std::string, bool> hideable_widgets_visibilities;

    for (std::map<std::string, QWidget*>::const_iterator widget_iterator =
         hideable_widgets_.begin();
         widget_iterator != hideable_widgets_.end();
         ++widget_iterator)
    {
        QWidget const* blub = this;
        QWidget* bla = const_cast<QWidget*>(blub);
        hideable_widgets_visibilities[widget_iterator->first] =
                widget_iterator->second->isVisibleTo (bla);
    }

    return hideable_widgets_visibilities;
}


//-----------------------------------------------------------------------------
void SignalBrowserView::setWidgetVisibility (std::string const &widget_name, bool visibility)
{
    std::map<std::string, QWidget*>::iterator widget_iterator = hideable_widgets_.find(widget_name);
    if (widget_iterator == hideable_widgets_.end())
        return;
    else
        widget_iterator->second->setVisible (visibility);
}

//-----------------------------------------------------------------------------
void SignalBrowserView::goTo (float32 x, float32 y)
{
    x += graphics_view_->width() / 2;
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
void SignalBrowserView::setViewCursor (QCursor const &cursor)
{
    graphics_view_->setCursor(cursor);
}


//-----------------------------------------------------------------------------
void SignalBrowserView::updateWidgets (bool update_view)
{
    if (update_view)
        graphics_view_->viewport()->update();
    y_axis_widget_->updateAllChannels();
    x_axis_widget_->update();
    label_widget_->update();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::setXAxisIntervall (float64 intervall)
{
    x_axis_widget_->changeIntervall (intervall);
}

//-----------------------------------------------------------------------------
void SignalBrowserView::setPixelPerSec (float64 pixel_per_sec)
{
    x_axis_widget_->changePixelPerSec (pixel_per_sec);
}

//-----------------------------------------------------------------------------
void SignalBrowserView::verticalSrollbarMoved(int)
{
    //y_axis_widget_->update();//repaint();
    label_widget_->update();//repaint();
    emit visibleYChanged (graphics_view_->mapToScene(0,0).y());
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
    //y_axis_widget_->repaint();
    label_widget_->update ();//repaint();
    vertical_scrollbar_->setRange(min, max);
    vertical_scrollbar_->setPageStep(graphics_view_->verticalScrollBar()->pageStep());
    emit visibleYChanged (graphics_view_->mapToScene(0,0).y());
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

//-------------------------------------------------------------------------
void SignalBrowserView::contextMenuEvent (QContextMenuEvent * event)
{
    QMenu* menu = new QMenu (this);
    for (std::map<std::string, QWidget*>::iterator widget_iter =
         hideable_widgets_.begin();
         widget_iter != hideable_widgets_.end();
         ++widget_iter)
    {
        QAction* action = new QAction (tr(widget_iter->first.c_str()), menu);
        action->setCheckable (true);
        action->setChecked (widget_iter->second->isVisibleTo(this));
        widget_iter->second->connect (action, SIGNAL(toggled(bool)), SLOT(setVisible (bool)));
        menu->addAction (action);
    }
    menu->exec(event->globalPos());
}


//-----------------------------------------------------------------------------
void SignalBrowserView::createLayout()
{
    layout_ = new QGridLayout(this);

    layout_->setMargin(0);
    layout_->setSpacing(0);
    layout_->setVerticalSpacing(0);
    layout_->setHorizontalSpacing(0);

    layout_->addWidget(event_info_widget_, 1, 2);
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
