#ifndef X_AXIS_WIDGET_H
#define X_AXIS_WIDGET_H

#include <QWidget>

namespace BioSig_
{

namespace PortingToQT4_
{


class XAxisWidget : public QWidget
{
public:
    XAxisWidget();
    virtual ~XAxisWidget();
    virtual QSize sizeHint () const;

private:
    virtual void paintEvent(QPaintEvent* event = 0);

};

}

}

#endif // X_AXIS_WIDGET_H
