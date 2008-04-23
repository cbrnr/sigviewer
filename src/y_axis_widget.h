// y_axis_widget.h



#ifndef Y_AXIS_WIDGET_H

#define Y_AXIS_WIDGET_H



#include "base/user_types.h"



#include <QMap>

#include <QWidget>

#include <QPaintEvent>



namespace BioSig_

{



class SignalBrowserModel;

class SignalBrowser;

class SignalCanvasItem;



// y axis widget

class YAxisWidget : public QWidget

{

public:

    YAxisWidget(SignalBrowserModel& model, SignalBrowser* browser);



    void addChannel(int32 channel_nr, SignalCanvasItem* signal_item);

    void removeChannel(int32 channel_nr);



    static void loadSettings();

    static void saveSettings();



    static const QColor& getAxisColor();



protected:

    void paintEvent(QPaintEvent* pe);



private:

    SignalBrowserModel& signal_browser_model_;

    SignalBrowser* signal_browser_;

    QMap<int32, SignalCanvasItem*> channel_nr2signal_canvas_item_;

    static QColor axis_color_;

};



} // namespace BioSig_



#endif

