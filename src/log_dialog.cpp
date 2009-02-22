// log_dialog.cpp

#include "log_dialog.h"

#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextStream>
#include <QSettings>

namespace BioSig_
{

// constructor
LogDialog::LogDialog(QTextStream& log_stream, QWidget* parent)
 : QDialog(parent)
{
    setWindowTitle(tr("Log"));
    QVBoxLayout* top_layout = new QVBoxLayout(this);
    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    text_edit_ = new QTextEdit(this);
    text_edit_->setLineWrapMode(QTextEdit::NoWrap);
    text_edit_->setReadOnly(true);
    text_edit_->append(*log_stream.string());
    top_layout->addWidget(text_edit_);
    QHBoxLayout* button_layout = new QHBoxLayout(top_layout);
    button_layout->setMargin(10);
    button_layout->addStretch(1);
    close_button_ = new QPushButton(tr("Close"), this);
    button_layout->addWidget(close_button_);
    button_layout->addStretch(1);
    resize(400, 400);
    top_layout->activate();
    connect(close_button_, SIGNAL(clicked()), this, SLOT(accept()));
}

// load settings
void LogDialog::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("LogDialog");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

// save settings
void LogDialog::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("LogDialog");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

} // namespace BioSig_
