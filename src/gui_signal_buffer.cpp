// gui_signal_buffer.cpp



#include "gui_signal_buffer.h"



#include <QProgressDialog>

#include <QApplication>



namespace BioSig_

{



// constructor init done event

GUISignalBuffer::InitDoneEvent::InitDoneEvent(uint32 percent, int32 status)

: QEvent(QEvent::User),

  percent_(percent),

  status_(status)

{

    // nothing

}



// init done event get percent

uint32 GUISignalBuffer::InitDoneEvent::getPercent() const

{

    return percent_;

}



// init done event get status

int32 GUISignalBuffer::InitDoneEvent::getStatus() const

{

    return status_;

}



// constructor

GUISignalBuffer::GUISignalBuffer(FileSignalReader& reader)

: SignalBuffer(reader),

  progress_dialog_(new QProgressDialog)

{

    progress_dialog_->setCancelButton(0);

    progress_dialog_->setRange(0, 100);

    progress_dialog_->setAutoClose(false);

}



// destructor

GUISignalBuffer::~GUISignalBuffer()

{

    delete progress_dialog_;

}



// init

void GUISignalBuffer::init()

{



    progress_dialog_->setWindowTitle(tr("Initialize Buffer"));

    progress_dialog_->setLabelText("");

    status_ = -1;

    start(); // LowPriority);

    progress_dialog_->exec();

}



// init done 

void GUISignalBuffer::initDone(uint32 percent, InitStatus status)

{

    QApplication::postEvent(this, new InitDoneEvent(percent, status));

}



// run

void GUISignalBuffer::run()

{

    SignalBuffer::init();

    QApplication::postEvent(this, new InitDoneEvent(101, -1));

}



// event

void GUISignalBuffer::customEvent(QEvent *event)

{

    InitDoneEvent* init_done_event = (InitDoneEvent*)event;

    if (status_ != init_done_event->getStatus())

    {

        status_ = init_done_event->getStatus();

        switch(status_)

        {

            case INIT_DOWNSAMPLE:

                progress_dialog_->setLabelText(tr("Downsampling Progress"));

                break;

            case INIT_MIN_MAX:

                progress_dialog_->setLabelText(tr("Range Finding Progress"));

                break;

            case INIT_EVENTS:

                progress_dialog_->setLabelText(tr("Loading Events Progress"));

                break;

        }

    }

    if (init_done_event->getPercent() <= 100)

    {

        progress_dialog_->setValue(init_done_event->getPercent());

    }

    else

    {

        progress_dialog_->close(); // all done

    }

}



} // namespace BioSig_

