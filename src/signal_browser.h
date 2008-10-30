// signal_browser.h
#ifndef SIGNAL_BROWSER_H
#define SIGNAL_BROWSER_H

#include "base/user_types.h"

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>

class QScrollBar;
class QVBoxLayout;
class Q3ScrollView;
class QLineEdit;
class QLabel;
class QResizeEvent;
class QScrollBar;
class QGridLayout;

namespace BioSig_
{

class SignalBrowserModel;
class SmartCanvas;
class SmartCanvasView;
class XAxisWidget;
class YAxisWidget;
class LabelWidget;

// signal browser
class SignalBrowser : public QFrame
{
    Q_OBJECT

public:

    enum
    {
        UPDATE_PERIOD = 20 // ms
    };

    SignalBrowser(SignalBrowserModel& model, QWidget* parent = 0);
    SmartCanvas* getCanvas();
    SmartCanvasView* getCanvasView();
    XAxisWidget* getXAxisWidget();
    YAxisWidget* getYAxisWidget();
    LabelWidget* getLabelWidget();

    //FIXME: remove this!
    void fixmeTestStackedLayout();
    void installLayout();
    //

protected:

    void resizeEvent(QResizeEvent* e);

private slots:

    void horizontalSrollbarMoved(int value);
    void verticalSrollbarMoved(int value);
    void updateScrollBarRange();

private:

	enum LAYOUTS
	{
		DEFAULT_LAYOUT = 0x00,
		CHANNEL_LABELS_ONLY_LAYOUT = 0x01
	};

    SignalBrowserModel& model_;
    SmartCanvas* canvas_;
    SmartCanvasView* canvas_view_;
    QScrollBar* h_scrollbar_;
    QScrollBar* v_scrollbar_;
    XAxisWidget* x_axis_widget_;
    YAxisWidget* y_axis_widget_;
    LabelWidget* label_widget_;

    QGridLayout* layout_;

    void createLayout();
};

} // namespace BioSig_

#endif

