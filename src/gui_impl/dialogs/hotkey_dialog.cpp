// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "hotkey_dialog.h"
#include "ui_hotkey_dialog.h"

HotkeyDialog::HotkeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HotkeyDialog)
{
    ui->setupUi(this);
    resize(633, 744);
    this->setWindowTitle(tr("Keyboard and Mouse Shortcuts"));

#if defined(Q_OS_MACOS)
    QString ctrl = "Cmd";
#else
    QString ctrl = "Ctrl";
#endif

    QStringList actionDescriptions;
    QStringList hotkeys;

    actionDescriptions << "Scale Individual Channel Up"
                       << "Scale Individual Channel Down"
                       << "Info Dialog"
                       << "Channel Dialog"
                       << "Event Dialog"
                       << "Open File"
                       << "Scale All"
                       << "Zoom In Horizontal"
                       << "Zoom Out Horizontal"
                       << "Zoom In Vertical"
                       << "Zoom Out Vertical"
                       << "New Event Mode"
                       << "Edit Event Mode"
                       << "Scroll Mode"
                       << "View Options Mode"
                       << "Exit";

    hotkeys << ctrl + "+Mousewheel Up"
            << ctrl + "+Mousewheel Down"
            << QKeySequence(tr("Ctrl+I")).toString()
            << QKeySequence(tr("Ctrl+C")).toString()
            << QKeySequence(tr("Ctrl+E")).toString()
            << QKeySequence(tr("Ctrl+O")).toString()
            << QKeySequence(tr("Ctrl+A")).toString()
            << QKeySequence(tr("Alt++")).toString()
            << QKeySequence(tr("Alt+-")).toString()
            << QKeySequence(tr("Ctrl++")).toString()
            << QKeySequence(tr("Ctrl+-")).toString()
            << QKeySequence(tr("Ctrl+1")).toString()
            << QKeySequence(tr("Ctrl+2")).toString()
            << QKeySequence(tr("Ctrl+3")).toString()
            << QKeySequence(tr("Ctrl+4")).toString()
            << QKeySequence(tr("Ctrl+F4")).toString();

    ui->tableWidget->setRowCount(actionDescriptions.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setColumnWidth(0, width() * 0.47);
    ui->tableWidget->setColumnWidth(1, width() * 0.47);

    for (int row = 0; row < ui->tableWidget->rowCount(); row++)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(actionDescriptions[row]));
        newItem->setFlags(newItem->flags()^Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, newItem);
        newItem = new QTableWidgetItem(tr("%1").arg(hotkeys[row]));
        newItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
        newItem->setFlags(newItem->flags()^Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 1, newItem);
    }
}

HotkeyDialog::~HotkeyDialog()
{
    delete ui;
}

void HotkeyDialog::on_pushButton_clicked()
{
    close();
}
