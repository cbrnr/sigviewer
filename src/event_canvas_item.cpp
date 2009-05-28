#ifndef QT4_PORTED
// event_canvas_item.cpp

#include "event_canvas_item.h"
#include "smart_canvas/smart_canvas_view.h"
#include "signal_browser.h"
#include "base/signal_event.h"
#include "main_window.h"
#include "main_window_model.h"
#include "event_color_manager.h"
#include "base/event_table_file_reader.h"
#include "signal_browser_mouse_handling.h"

#include <q3popupmenu.h>

#include <QPainter>
#include <QEvent>
#include <QCursor>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QSettings>

#define min(a, b) ((a) < (b) ? (a) : (b))

namespace BioSig_
{

// move mouse range
int EventCanvasItem::move_mouse_range_ = 5;

// load settings
void EventCanvasItem::loadSettings()
{
    QSettings settings("SigViewer");
    move_mouse_range_ = settings.value("EventCanvasItem/move_mouse_range",
                                       move_mouse_range_).toInt();
}

// save settings
void EventCanvasItem::saveSettings()
{
    QSettings settings("SigViewer");
    settings.setValue("EventCanvasItem/move_mouse_range", move_mouse_range_);
}

// constructor
EventCanvasItem::EventCanvasItem(uint32 id, SignalBuffer& buffer,
                                 SignalBrowserModel& model,
                                 SignalBrowser* browser)
: SmartCanvasRectangle(browser->getCanvas()),
  signal_buffer_(buffer),
  signal_browser_model_(model),
  signal_browser_(browser),
  id_(id),
  state_(STATE_NONE),
  canvas_view_(0)
{
    EventColorManager& event_color_manager
        = signal_browser_model_.getMainWindowModel().getEventColorManager();
    color_ = event_color_manager.getEventColor(signal_buffer_.getEvent(id_)
                                                                ->getType());
}

// destructor
EventCanvasItem::~EventCanvasItem()
{
    if (canvas_view_)
    {
        canvas_view_->removeEventListener(SmartCanvasView::ALL_EVENTS,
                                          this);
    }
}

// get id
uint32 EventCanvasItem::getId() const
{
    return id_;
}

// rtti
int EventCanvasItem::rtti() const
{
    return RTTI;
}

// get color
const QColor& EventCanvasItem::getColor() const
{
    return color_;
}

// update color
void EventCanvasItem::updateColor()
{
    EventColorManager& event_color_manager
        = signal_browser_model_.getMainWindowModel().getEventColorManager();
    color_ = event_color_manager.getEventColor(signal_buffer_.getEvent(id_)
                                                                ->getType());
}

// smart draw
void EventCanvasItem::smartDraw(QPainter& p, const QRect& clip)
{
    if (isSelected())
    {
        p.drawRect(rect());
    }
    p.fillRect(clip & rect(), color_);
}

// mouse press event
void EventCanvasItem::mousePressEvent(QMouseEvent* e, Q3CanvasItemList&, int32,
                                      SmartCanvasView* canvas_view)
{
    if (!canvas_view_)
    {
        canvas_view_ = canvas_view;
    }

    if (state_ != STATE_NONE)
    {
        return; // already in a edit state
    }

    switch(getMousePressAction(e, canvas_view))
    {
        case ACTION_NONE:
            break;
        case ACTION_MOVE_BEGIN:
            state_ = STATE_MOVE_BEGIN;
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            signal_browser_->getCanvasView()->viewport()
                                ->setCursor(QCursor(Qt::SizeHorCursor));
            break;
        case ACTION_MOVE_END:
            state_ = STATE_MOVE_END;
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            signal_browser_->getCanvasView()->viewport()
                                ->setCursor(QCursor(Qt::SizeHorCursor));
            break;
        case ACTION_SHIFT_TO_CHANNEL:
            state_ = STATE_SHIFT_TO_CHANNEL;
            last_shift_shown_nr_
                = (int32)((rect().y() + rect().height() / 2) /
                          (signal_browser_model_.getSignalHeight() +
                           signal_browser_model_.getSignalSpacing()));
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            signal_browser_->getCanvasView()->viewport()
                                        ->setCursor(QCursor(Qt::SizeVerCursor));
            break;
        case ACTION_COPY_SHIFT_TO_CHANNEL:
            {
                SignalEvent* event = signal_buffer_.getEvent(id_);
                SignalEvent new_event(*event);
                signal_browser_model_.addEvent(new_event);
                state_ = STATE_SHIFT_TO_CHANNEL;
                last_shift_shown_nr_
                    = (int32)((rect().y() + rect().height() / 2) /
                              (signal_browser_model_.getSignalHeight() +
                               signal_browser_model_.getSignalSpacing()));
                canvas_view->addEventListener(
                                        SmartCanvasView::MOUSE_RELEASE_EVENT |
                                        SmartCanvasView::MOUSE_MOVE_EVENT,
                                        this);
                signal_browser_->getCanvasView()->viewport()
                                        ->setCursor(QCursor(Qt::SizeVerCursor));
            }
            break;
        case ACTION_SELECT:
            {
                EventCanvasItem* old_selected_item
                    = signal_browser_model_.getSelectedEventItem();
                if (old_selected_item)
                {
                    canvas_view->removeEventListener(
                                        SmartCanvasView::MOUSE_PRESS_EVENT,
                                        old_selected_item);
                }
                canvas_view->addEventListener(
                                        SmartCanvasView::MOUSE_PRESS_EVENT,
                                        this);
                signal_browser_model_.setSelectedEventItem(this);
            }
            break;
    }
}

// mouse release event
void EventCanvasItem::mouseReleaseEvent(QMouseEvent*, Q3CanvasItemList&,
                                        int32, SmartCanvasView* canvas_view)
{
    switch(state_)
    {
        case STATE_NONE:
            break;
        case STATE_MOVE_BEGIN:
            canvas_view->removeEventListener(
                                        SmartCanvasView::MOUSE_RELEASE_EVENT |
                                        SmartCanvasView::MOUSE_MOVE_EVENT,
                                        this);
            signal_browser_->getCanvasView()->viewport()
                                    ->setCursor(QCursor(Qt::ArrowCursor));
            state_ = STATE_NONE;
            break;
        case STATE_MOVE_END:
            canvas_view->removeEventListener(
                                        SmartCanvasView::MOUSE_RELEASE_EVENT |
                                        SmartCanvasView::MOUSE_MOVE_EVENT,
                                        this);
            signal_browser_->getCanvasView()->viewport()
                                    ->setCursor(QCursor(Qt::ArrowCursor));
            state_ = STATE_NONE;
            break;
        case STATE_SHIFT_TO_CHANNEL:
            canvas_view->removeEventListener(
                                        SmartCanvasView::MOUSE_RELEASE_EVENT |
                                        SmartCanvasView::MOUSE_MOVE_EVENT,
                                        this);
            signal_browser_->getCanvasView()->viewport()
                                    ->setCursor(QCursor(Qt::ArrowCursor));
            state_ = STATE_NONE;
            break;
    }
}

// mouse move event
void EventCanvasItem::mouseMoveEvent(QMouseEvent* e, Q3CanvasItemList&,
                                     SmartCanvasView* canvas_view)
{
    SignalEvent* event = signal_buffer_.getEvent(id_);
    QPoint mouse_pos = canvas_view->inverseWorldMatrix().map(e->pos());
    switch(state_)
    {
        case STATE_NONE:
            break;
        case STATE_MOVE_BEGIN:
            {
                int32 end_pos = event->getPosition() + event->getDuration();
                int32 pos = (int32)(mouse_pos.x() /
                                    signal_browser_model_.getPixelPerSec() *
                                    signal_buffer_.getEventSamplerate());
                event->setPosition(pos < end_pos ? pos : end_pos);
                event->setDuration(end_pos - event->getPosition());
                signal_browser_model_.setEventChanged(id_);
            }
            break;
        case STATE_MOVE_END:
            {
                int32 dur = (int32)(mouse_pos.x() /
                                    signal_browser_model_.getPixelPerSec() *
                                    signal_buffer_.getEventSamplerate()) -
                            event->getPosition();
                event->setDuration(dur > 0 ? dur : 0);
                signal_browser_model_.setEventChanged(id_);
            }
            break;
        case STATE_SHIFT_TO_CHANNEL:
            int32 shown_nr;
            shown_nr = (int32)(mouse_pos.y() /
                               (signal_browser_model_.getSignalHeight() +
                                signal_browser_model_.getSignalSpacing()));
            if (shown_nr != last_shift_shown_nr_)
            {
                event->setChannel(signal_browser_model_.getChannelNr(shown_nr));
                signal_browser_model_.setEventChanged(id_);
                last_shift_shown_nr_ = shown_nr;
            }
            break;
    }
}

// tool tip text
void EventCanvasItem::toolTipText(const QPoint&, Q3CanvasItemList&,
                                  QString* tool_tip)
{
    float64 event_sample_rate;
    event_sample_rate = signal_buffer_.getEventSamplerate();
    SignalEvent* event = signal_buffer_.getEvent(id_);
    EventTableFileReader& event_table_reader
        = signal_browser_model_.getMainWindowModel().getEventTableFileReader();
    QString event_name = event_table_reader.getEventName(event->getType());
    if (event_name.isEmpty())
    {
        event_name = "Undefined";
    }
    *tool_tip += QString("<nobr><font color=%1><b> %2</b> ").arg(color_.name())
                                                            .arg(event_name);
    *tool_tip += QString("<i>(0x%1)</i></font></nobr><br>").arg(
                                                               event->getType(),
                                                               4,16)
                                                           .replace(' ', '0');
    *tool_tip += QString("<nobr>Position: %1, ").arg(event->getPosition() /
                                                     event_sample_rate);
    *tool_tip += QString("Duration: %1</nobr><br>").arg(event->getDuration() /
                                                        event_sample_rate);
}

// context menu event
void EventCanvasItem::contextMenuEvent(QContextMenuEvent*, Q3CanvasItemList&,
                                       int32, Q3PopupMenu* menu,
                                       SmartCanvasView*)
{
    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    if (mode != SignalBrowserModel::MODE_POINTER &&
        mode != SignalBrowserModel::MODE_NEW)
    {
        return;
    }

    if (menu->count() > 0)
    {
        menu->insertSeparator();
    }

    EventCanvasItem* old_selected_item;
    old_selected_item = signal_browser_model_.getSelectedEventItem();
    if (old_selected_item == this)
    {
        MainWindow* main_window
            = signal_browser_model_.getMainWindowModel().getMainWindow();
        main_window->addActionTo(menu, "edit_to_all_channels_action_");
        main_window->addActionTo(menu, "edit_copy_to_channels_action_");
        main_window->addActionTo(menu, "edit_delete_action_");
        main_window->addActionTo(menu, "edit_change_channel_action_");
        main_window->addActionTo(menu, "edit_change_type_action_");
    }
    else
    {
        SignalEvent* event = signal_buffer_.getEvent(id_);
        EventTableFileReader& event_table_reader
         = signal_browser_model_.getMainWindowModel().getEventTableFileReader();
        QString event_name = event_table_reader.getEventName(event->getType());
        if (event_name.isEmpty())
        {
            event_name = "Undefined";
        }
        menu->insertItem(tr("&Select ") + event_name, this, SLOT(select()));
    }
}

// select
void EventCanvasItem::select()
{
    signal_browser_model_.setSelectedEventItem(this);
}

// get mouse press action
EventCanvasItem::Action
    EventCanvasItem::getMousePressAction(QMouseEvent* e,
                                         SmartCanvasView* canvas_view)
{
    SignalEvent* event = signal_buffer_.getEvent(id_);
    EventCanvasItem* old_selected_item
        = signal_browser_model_.getSelectedEventItem();
    QPoint mouse_pos = canvas_view->inverseWorldMatrix().map(e->pos());

    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    switch (SignalBrowserMouseHandling::getAction(e, mode))
    {
        case SignalBrowserMouseHandling::SELECT_RESIZE_EVENT_ACTION:
            {
                // select event
                if (!old_selected_item)
                {
                    return ACTION_SELECT;
                }             

                QRect old_rect = old_selected_item->rect();
                int32 tmp = min(old_rect.width() / 2, move_mouse_range_);
        
                // move event end
                QRect move_end_rect(old_rect.right() - tmp, old_rect.top(),
                                    tmp + move_mouse_range_, old_rect.height());
                if (this == old_selected_item &&
                    move_end_rect.contains(mouse_pos))
                {
                    return ACTION_MOVE_END;
                }

                // move event begin
                QRect move_begin_rect(old_rect.left() - move_mouse_range_,
                                      old_rect.top(), tmp + move_mouse_range_,
                                      old_rect.height());
                if (this == old_selected_item &&
                    move_begin_rect.contains(mouse_pos))
                {
                    return ACTION_MOVE_BEGIN;
                }

                // select smallest clicked event
                if (old_selected_item != this &&
                    (!old_rect.contains(mouse_pos) ||
                     old_selected_item->rect().width() > rect().width() ||
                     (old_selected_item->rect().width() == rect().width() &&
                      old_selected_item->rect().height() > rect().height())))
                {
                    return ACTION_SELECT;
                }        
            }
            break;
        case SignalBrowserMouseHandling::SHIFT_EVENT_TO_CHANNEL_ACTION:
            if (this == old_selected_item && 
                old_selected_item->rect().contains(mouse_pos) &&
                event->getChannel() != SignalEvent::UNDEFINED_CHANNEL)
            {
                return ACTION_SHIFT_TO_CHANNEL;
            }
            break;
        case SignalBrowserMouseHandling::COPY_EVENT_TO_CHANNEL_ACTION:
            if (this == old_selected_item &&
                old_selected_item->rect().contains(mouse_pos) &&
                event->getChannel() != SignalEvent::UNDEFINED_CHANNEL)
            {
                return ACTION_COPY_SHIFT_TO_CHANNEL;
            }
            break;
        default:
            break;
    }
    return ACTION_NONE;
}

// start mouse move end
void EventCanvasItem::startMouseMoveEnd(SmartCanvasView* canvas_view)
{
    state_ = STATE_MOVE_END;
    canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                  SmartCanvasView::MOUSE_MOVE_EVENT, this);
    signal_browser_->getCanvasView()->viewport()
                                    ->setCursor(QCursor(Qt::SizeHorCursor));
}

} // namespace BioSig_

#endif
