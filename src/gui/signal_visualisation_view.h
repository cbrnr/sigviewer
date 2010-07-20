#ifndef SIGNAL_VISUALISATION_VIEW_H
#define SIGNAL_VISUALISATION_VIEW_H

#include <QPainter>
#include <QSharedPointer>
#include <QImage>

namespace BioSig_
{

class SignalVisualisationView
{
public:
    virtual QSharedPointer<QImage> renderVisibleScene () const = 0;

    virtual double getYGridValueInterval () const = 0;

    virtual bool getXAxisVisibility () const = 0;
    virtual bool getYAxisVisibility () const = 0;
    virtual bool getLabelsVisibility () const = 0;
};

}

#endif // SIGNAL_VISUALISATION_VIEW_H
