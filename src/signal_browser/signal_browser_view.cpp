// signal_browser.cpp

#include "signal_browser_view.h"
#include "signal_browser_model.h"
#include "signal_graphics_item.h"
#include "event_graphics_item.h"
#include "y_axis_widget_4.h"
#include "x_axis_widget_4.h"
#include "../label_widget.h"

#include <QGraphicsLineItem>
#include <QGridLayout>
#include <QScrollBar>
#include <QPointF>

#include <iostream>


namespace BioSig_
{

namespace PortingToQT4_
{

//-----------------------------------------------------------------------------
// constructor
SignalBrowserView::SignalBrowserView(SignalBrowserModel* signal_browser_model, QWidget* parent)
: QFrame(parent)
{
    resize(parent->contentsRect().width(), parent->contentsRect().height());
    graphics_scene_ = new QGraphicsScene (0,0,parent->contentsRect().width(), parent->contentsRect().height(), this);
    graphics_view_ = new QGraphicsView(graphics_scene_, this);
    graphics_view_->scroll(0,0);
    graphics_view_->horizontalScrollBar()->hide();
    graphics_view_->verticalScrollBar()->hide();
    graphics_view_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphics_view_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    graphics_view_->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    y_axis_widget_ = new YAxisWidget (this, *signal_browser_model, this);
    y_axis_widget_->resize(70, height());
    y_axis_widget_->setMinimumSize(70, 0);

    x_axis_widget_ = new XAxisWidget (this, *signal_browser_model, this);
    x_axis_widget_->resize(width()-300, 30);
    x_axis_widget_->setMinimumSize(0, 30);

#ifdef QT4_PORTED
    label_widget_ = new LabelWidget (*signal_browser_model, this);
    label_widget_->resize(70, height());
    label_widget_->setMinimumWidth(70);
#endif

    horizontal_scrollbar_ = new QScrollBar (Qt::Horizontal, this);
    vertical_scrollbar_ = new QScrollBar (Qt::Vertical, this);

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


    graphics_view_->resize(width() - label_widget_->width() - y_axis_widget_->width() + (vertical_scrollbar_->width()*2), height() - x_axis_widget_->height() + horizontal_scrollbar_->height());

    // graphics_view_->setVerticalScrollBarPolicy(setViewportMargins(0, 0, 0, 0);
    //graphics_view_->setOptimizationFlag(QGraphicsView::DontClipPainter, true);
    graphics_view_->setMinimumSize(0, 0);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    createLayout();
}

//-----------------------------------------------------------------------------
SignalBrowserView::~SignalBrowserView ()
{

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

//    graphics_scene_->removeItem(graphics_item->childItems());
    graphics_scene_->removeItem(graphics_item);
    //QGraphicsLineItem* signal_separator_item = new QGraphicsLineItem (0, graphics_item->boundingRect().bottom(), graphics_item->boundingRect().right(), graphics_item->boundingRect().bottom(), graphics_item);
    graphics_scene_->addItem(graphics_item);
    //    int separator_y = graphics_item->pos().y() + graphics_item->boundingRect().height() + 1;
//    graphics_scene_->addLine(0, separator_y, graphics_item->boundingRect().width(),separator_y);
    y_axis_widget_->addChannel(channel_nr, graphics_item);
    label_widget_->addChannel(channel_nr, QString::number(channel_nr));
    // graphics_scene_->addLine(200, 0, 200, 200);

    graphics_view_->update();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::removeSignalGraphicsItem (int32 channel_nr)
{
//    graphics_scene_->removeItem(graphics_item);
//    graphics_scene_->addItem(graphics_item);
//    y_axis_widget_->addChannel(channel_nr, graphics_item);
//    graphics_view_->update();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::addEventGraphicsItem (QSharedPointer<EventGraphicsItem> event_graphics_item)
{
    // TODO: really remove before add????
    graphics_scene_->removeItem(event_graphics_item.data());
    graphics_scene_->addItem(event_graphics_item.data());

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
YAxisWidget& SignalBrowserView::getYAxisWidget () const
{
    return *y_axis_widget_;
}

//-----------------------------------------------------------------------------
XAxisWidget& SignalBrowserView::getXAxisWidget () const
{
    return *x_axis_widget_;
}


//-----------------------------------------------------------------------------
LabelWidget& SignalBrowserView::getLabelWidget () const
{
    return *label_widget_;
}

//-----------------------------------------------------------------------------
void SignalBrowserView::goTo (int32 x, int32 y)
{
    graphics_view_->centerOn(x, y);
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
    y_axis_widget_->update();
    x_axis_widget_->update();
    label_widget_->update();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::verticalSrollbarMoved(int value)
{
    y_axis_widget_->repaint();
    label_widget_->repaint();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::horizontalSrollbarMoved(int value)
{
    x_axis_widget_->repaint();
}

//-----------------------------------------------------------------------------
void SignalBrowserView::horizontalScrollBarRangeChaned (int min, int max)
{
    x_axis_widget_->repaint();
    horizontal_scrollbar_->setRange(min, max);
    horizontal_scrollbar_->setPageStep(graphics_view_->horizontalScrollBar()->pageStep());
}

//-----------------------------------------------------------------------------
void SignalBrowserView::verticalScrollBarRangeChaned (int min, int max)
{
    y_axis_widget_->repaint();
    label_widget_->repaint();
    vertical_scrollbar_->setRange(min, max);
    vertical_scrollbar_->setPageStep(graphics_view_->verticalScrollBar()->pageStep());
}

//-----------------------------------------------------------------------------
void SignalBrowserView::createLayout()
{
    layout_ = new QGridLayout(this);

    layout_->setMargin(0);
    layout_->setSpacing(0);
    layout_->setVerticalSpacing(0);
    layout_->setHorizontalSpacing(0);

    layout_->addWidget(y_axis_widget_, 1, 1);
    layout_->addWidget(graphics_view_, 1, 2);
    layout_->addWidget(x_axis_widget_, 2, 2);
    layout_->addWidget(horizontal_scrollbar_, 3, 2);
    layout_->addWidget(label_widget_, 1, 3);
    layout_->addWidget(vertical_scrollbar_, 1, 4);
//    layout_->addWidget(label_widget_, 1, 3);
//    layout_->addWidget(x_axis_widget_, 2, 2);
//    layout_->addWidget(h_scrollbar_, 3, 1, 1, 3);
//    layout_->addWidget(v_scrollbar_, 1, 4, 2, 1);
//
//    QWidget* dummy = new QWidget(this);
//    dummy->setBackgroundColor(XAxisWidget::getAxisColor());
//    layout_->addWidget(dummy, 2, 1);
//
//    dummy = new QWidget(this);
//    dummy->setBackgroundColor(XAxisWidget::getAxisColor());
//    layout_->addWidget(dummy, 2, 3);
}
/*
// get canvas
SmartCanvas* SignalBrowserView::getCanvas()
{
    return canvas_;
}

// get canvas view
SmartCanvasView* SignalBrowserView::getCanvasView()
{
    return canvas_view_;
}

// get x axis widget
XAxisWidget* SignalBrowserView::getXAxisWidget()
{
    return x_axis_widget_;
}

// get y axis widget
YAxisWidget* SignalBrowserView::getYAxisWidget()
{
    return y_axis_widget_;
}

// get label widget
LabelWidget* SignalBrowserView::getLabelWidget()
{
    return label_widget_;
}

// resize event
void SignalBrowserView::resizeEvent(QResizeEvent* e)
{
    updateScrollBarRange();
}

// update scrollbar range
void SignalBrowserView::updateScrollBarRange()
{
    int32 visible_width = canvas_view_->visibleWidth();
    int32 visible_height = canvas_view_->visibleHeight();
    int32 h_max = canvas_->width() - visible_width;
    int32 v_max = canvas_->height() - visible_height;

    h_scrollbar_->setRange(0, h_max > 0 ? h_max : 0);
    h_scrollbar_->setSteps(10, visible_width);
    v_scrollbar_->setRange(0, v_max > 0 ? v_max : 0);
    v_scrollbar_->setSteps(10, visible_height);
    x_axis_widget_->update();
}

// horizontal scrollbar moved
void SignalBrowserView::horizontalSrollbarMoved(int)
{
    x_axis_widget_->repaint();
}

// vertical scrollbar moved
void SignalBrowserView::verticalSrollbarMoved(int)
{
    label_widget_->repaint();
    y_axis_widget_->repaint();
}
*/
} //namespace PortingToQT4_

} // namespace BioSig_

