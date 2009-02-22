// log_dialog.h

#ifndef LOG_DIALOG_H
#define LOG_DIALOG_H

#include <QDialog>

class QTextEdit;
class QPushButton;
class QTextStream;

namespace BioSig_
{

// log dialog
class LogDialog : public QDialog
{
    Q_OBJECT
public:
    LogDialog(QTextStream& log_stream, QWidget* parent = 0);

    void loadSettings();
    void saveSettings();

private:
    // not allowed
    LogDialog();
    LogDialog(const LogDialog&);
    const LogDialog& operator=(const LogDialog&);

    QTextEdit* text_edit_;
    QPushButton* close_button_;
};

} // namespace BioSig_

#endif
