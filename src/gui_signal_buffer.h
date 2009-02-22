// gui_signal_buffer.h

#ifndef GUI_SIGNAL_BUFFER_H
#define GUI_SIGNAL_BUFFER_H

#include "base/signal_buffer.h"

#include <QThread>
#include <QEvent>

class QProgressDialog;

namespace BioSig_
{

// GUI signal buffer
class GUISignalBuffer : private QThread,
                        public SignalBuffer
{
    Q_OBJECT
public:
    GUISignalBuffer(FileSignalReader& reader);
    ~GUISignalBuffer();

    void init();

protected:
    void run();
    void initDone(uint32 percent, InitStatus status);

    void customEvent (QEvent *event);

private:
    class InitDoneEvent : public QEvent
    {
    public:
        InitDoneEvent(uint32 percent, int32 status);

        uint32 getPercent() const;
        int32 getStatus() const;

    private:
        uint32 percent_;
        int32 status_;
    };

    QProgressDialog* progress_dialog_;
    int32 status_;
};

} // namespace BioSig_

# endif
