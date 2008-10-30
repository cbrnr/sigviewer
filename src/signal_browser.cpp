// signal_browser.cpp

#include "signal_browser.h"
#include "signal_browser_model.h"
#include "smart_canvas/smart_canvas.h"
#include "smart_canvas/smart_canvas_view.h"
#include "x_axis_widget.h"
#include "y_axis_widget.h"
#include "label_widget.h"

#include <QScrollBar>
#include <QGridLayout>
#include <QResizeEvent>
#include <QWidget>

namespace BioSig_
{

// constructor
SignalBrowser::SignalBrowser(SignalBrowserModel& model, QWidget* parent)
: QFrame(parent),
  model_(model)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    canvas_ = new SmartCanvas(this, false);
    canvas_view_ = new SmartCanvasView(canvas_, this);
    canvas_view_->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    canvas_view_->setVScrollBarMode(Q3ScrollView::AlwaysOff);
    canvas_view_->setFrameStyle(Q3Frame::NoFrame);
    h_scrollbar_ = new QScrollBar(Qt::Horizontal, this);
    v_scrollbar_ = new QScrollBar(Qt::Vertical, this);
    x_axis_widget_ = new XAxisWidget(model, this);
    y_axis_widget_ = new YAxisWidget(model, this);
    label_widget_ = new LabelWidget(model, this);

    createLayout();

    connect(canvas_view_, SIGNAL(contentsMoving(int, int)),
            &model, SLOT(canvasViewContentMoving(int, int)));
    connect(canvas_, SIGNAL(resized()), this, SLOT(updateScrollBarRange()));
    connect(canvas_view_->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            h_scrollbar_, SLOT(setValue(int)));
    connect(h_scrollbar_, SIGNAL(valueChanged(int)),
            canvas_view_->horizontalScrollBar(), SLOT(setValue(int)));
    connect(canvas_view_->verticalScrollBar(), SIGNAL(valueChanged(int)),
            v_scrollbar_, SLOT(setValue(int)));
    connect(v_scrollbar_, SIGNAL(valueChanged(int)),
            canvas_view_->verticalScrollBar(), SLOT(setValue(int)));
    connect(h_scrollbar_, SIGNAL(valueChanged(int)),
            this, SLOT(horizontalSrollbarMoved(int)));
    connect(v_scrollbar_, SIGNAL(valueChanged(int)),
            this, SLOT(verticalSrollbarMoved(int)));
}

void SignalBrowser::createLayout()
{
    layout_ = new QGridLayout(this);

    layout_->setMargin(1);
    layout_->setSpacing(0);

    layout_->addWidget(y_axis_widget_, 1, 1);
    layout_->addWidget(canvas_view_, 1, 2);

    layout_->addWidget(label_widget_, 1, 3);
    layout_->addWidget(x_axis_widget_, 2, 2);
    layout_->addWidget(h_scrollbar_, 3, 1, 1, 3);
    layout_->addWidget(v_scrollbar_, 1, 4, 2, 1);

    QWidget* dummy = new QWidget(this);
    dummy->setBackgroundColor(XAxisWidget::getAxisColor());
    layout_->addWidget(dummy, 2, 1);

    dummy = new QWidget(this);
    dummy->setBackgroundColor(XAxisWidget::getAxisColor());
    layout_->addWidget(dummy, 2, 3);
}

// get canvas
SmartCanvas* SignalBrowser::getCanvas()
{
    return canvas_;
}

// get canvas view
SmartCanvasView* SignalBrowser::getCanvasView()
{
    return canvas_view_;
}

// get x axis widget
XAxisWidget* SignalBrowser::getXAxisWidget()
{
    return x_axis_widget_;
}

// get y axis widget
YAxisWidget* SignalBrowser::getYAxisWidget()
{
    return y_axis_widget_;
}

// get label widget
LabelWidget* SignalBrowser::getLabelWidget()
{
    return label_widget_;
}

// resize event
void SignalBrowser::resizeEvent(QResizeEvent* e)
{
    updateScrollBarRange();
}

// update scrollbar range
void SignalBrowser::updateScrollBarRange()
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
void SignalBrowser::horizontalSrollbarMoved(int)
{
    x_axis_widget_->repaint();
}

// vertical scrollbar moved
void SignalBrowser::verticalSrollbarMoved(int)
{
    label_widget_->repaint();
    y_axis_widget_->repaint();
}

} // namespace BioSig_

