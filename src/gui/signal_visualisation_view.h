// © SigViewer developers
//
// License: GPL-3.0

#ifndef SIGNAL_VISUALISATION_VIEW_H
#define SIGNAL_VISUALISATION_VIEW_H

#include <QImage>
#include <QPainter>
#include <QSharedPointer>

namespace sigviewer {

class SignalVisualisationView {
   public:
    virtual QSharedPointer<QImage> renderVisibleScene() const = 0;

    virtual bool getXAxisVisibility() const = 0;
    virtual bool getYAxisVisibility() const = 0;
    virtual bool getLabelsVisibility() const = 0;

    virtual int getViewportHeight() const = 0;
    virtual int getViewportWidth() const = 0;
};

}  // namespace sigviewer

#endif
