#ifndef ZOOM_GUI_COMMAND_H
#define ZOOM_GUI_COMMAND_H

#include "../gui/gui_action_command.h"
#include "../gui/gui_action_factory_registrator.h"

namespace BioSig_
{

class ZoomGuiCommand : public GuiActionCommand
{
    Q_OBJECT
public:
    ZoomGuiCommand ();

    virtual void init ();

protected:
    //-------------------------------------------------------------------------
    virtual void evaluateEnabledness ();

private slots:
    //-------------------------------------------------------------------------
    void goTo ();

    //-------------------------------------------------------------------------
    void zoomInHorizontal ();

    //-------------------------------------------------------------------------
    void zoomOutHorizontal ();

    //-------------------------------------------------------------------------
    void zoomInVertical ();

    //-------------------------------------------------------------------------
    void zoomOutVertical ();

    //-------------------------------------------------------------------------
    void autoZoomVertical ();

private:
    //-------------------------------------------------------------------------
    unsigned maxSignalHeight ();

    //-------------------------------------------------------------------------
    unsigned minSignalHeight ();

    //-------------------------------------------------------------------------
    float32 maxPixelPerSample ();

    //-------------------------------------------------------------------------
    float32 minPixelPerSample ();


    static float const ZOOM_FACTOR_ = 2;
    static float const MAX_HORIZONTAL_ZOOM_IN_ = 32;

    static QString const GOTO_;
    static QString const ZOOM_IN_VERTICAL_;
    static QString const ZOOM_OUT_VERTICAL_;
    static QString const ZOOM_IN_HORIZONTAL_;
    static QString const ZOOM_OUT_HORIZONTAL_;
    static QString const AUTO_ZOOM_VERTICAL_;
    static QStringList const ACTIONS_;

    static GuiActionFactoryRegistrator registrator_;
};

}

#endif // ZOOM_GUI_COMMAND_H
