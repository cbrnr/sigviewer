// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "hotkey_dialog.h"
#include "ui_hotkey_dialog.h"

#include <QDesktopWidget>

HotkeyDialog::HotkeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HotkeyDialog)
{
    ui->setupUi(this);

    QDesktopWidget dw;
    int x=dw.width()*0.35;
    int y=dw.height()*0.78;
    setFixedSize(x,y);

    this->setWindowTitle(tr("Keyboard and Mouse Shortcuts"));

#if defined(Q_OS_MACOS)
    QString ctrl = "Cmd";
    QString alt = "Option";
    QString quit = "+Q"
#else
    QString ctrl = "Ctrl";
    QString alt = "Alt";
    QString quit = "+F4";
#endif

    QStringList actionDescriptions;
    QStringList hotkeys;

    actionDescriptions << "Scale Individual Channel Up"
                       << "Scale Individual Channel Down"
                       << "Scale All Channels Up"
                       << "Scale All Channels Down"
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
                       << "Undo"
                       << "Redo"
                       << "Close File"
                       << "Exit";

    hotkeys << ctrl + "+Mousewheel Up"
            << ctrl + "+Mousewheel Down"
            << "Shift + Mousewheel Up"
            << "Shift + Mousewheel Down"
            << ctrl + "+I"
            << ctrl + "+C"
            << ctrl + "+E"
            << ctrl + "+O"
            << ctrl + "+A"
            << alt  + "++"
            << alt  + "+-"
            << ctrl + "++"
            << ctrl + "+-"
            << ctrl + "+1"
            << ctrl + "+2"
            << ctrl + "+3"
            << ctrl + "+4"
            << ctrl + "+Z"
            << ctrl + "+Y"
            << ctrl + "+W"
            << ctrl + quit;

    ui->tableWidget->setRowCount(actionDescriptions.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setColumnWidth(0, width() * 0.47);
    ui->tableWidget->setColumnWidth(1, width() * 0.45);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

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
