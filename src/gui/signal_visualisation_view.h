// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef SIGNAL_VISUALISATION_VIEW_H
#define SIGNAL_VISUALISATION_VIEW_H

#include "gui_impl/signal_browser/signal_browser_graphics_view.h"

#include <QPainter>
#include <QSharedPointer>
#include <QImage>

namespace sigviewer
{

class SignalVisualisationView
{
public:
    virtual QSharedPointer<QImage> renderVisibleScene () const = 0;

    virtual bool getXAxisVisibility () const = 0;
    virtual bool getYAxisVisibility () const = 0;
    virtual bool getLabelsVisibility () const = 0;

    virtual int getViewportHeight () const = 0;
    virtual int getViewportWidth () const = 0;
    virtual SignalBrowserGraphicsView* getGraphicsView () const = 0;
};

}

#endif
