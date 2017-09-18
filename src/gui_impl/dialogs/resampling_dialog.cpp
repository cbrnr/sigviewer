// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "resampling_dialog.h"
#include "ui_resampling_dialog.h"
#include "file_handling_impl/xdf_reader.h"

#include <QDesktopWidget>

namespace sigviewer {

ResamplingDialog::ResamplingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResamplingDialog)
{
    ui->setupUi(this);
}

ResamplingDialog::ResamplingDialog(int nativeSrate, int highestSampleRate, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResamplingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Resampling");

    QDesktopWidget dw;
    int x=dw.width()*0.35;
    int y=dw.height()*0.6;
    resize(x,y);


    if (XDFdata->sampleRateMap.size() > 1)
    {
        QString text = "This file contains signals of multiple sample rates.<br> "
                       "Sigviewer needs to resample all channels to a unified sample rate in order to display them.<br> "
                       "Please choose a sample rate below (This won't change the actual file content):";
        ui->label->setText(text);
    }
    else if (XDFdata->sampleRateMap.size() == 1 &&
             XDFdata->sampleRateMap.count(0))
    {
        ui->label->setText("The nominal sample rate of this file is 0.\n"
                           "Please choose a preferred sample rate:");
    }
    else
    {
        QString text = "Would you like to resample this file? (This won't change the actual file content) ";
        ui->label->setText(text);
    }
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setColumnWidth(0, this->width() * 0.48);
    ui->treeWidget->setAnimated(true);
    QStringList headers;
    headers << "Stream" << "Info";
    ui->treeWidget->setHeaderLabels(headers);
    for (size_t i = 0; i < XDFdata->streams.size(); i++)
    {
        QTreeWidgetItem* streamItem = new QTreeWidgetItem(ui->treeWidget);
        streamItem->setText(0, "Stream "+QString::number(i+1));//+1 for user's convenience (1 based instead 0 based)
//        streamItem->setIcon(0, QIcon(":/images/ic_flag_black_24dp.png"));

        QTreeWidgetItem* infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Name"));
        infoItem->setText(1, QString::fromStdString(XDFdata->streams[i].info.name));

        infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Type"));
        infoItem->setText(1, QString::fromStdString(XDFdata->streams[i].info.type));

        infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Sample Rate"));
        infoItem->setText(1, QString::number(XDFdata->streams[i].info.nominal_srate).append(tr(" Hz")));
        if (XDFdata->streams[i].info.nominal_srate == 0)
            infoItem->setText(1, tr("Irregular"));

        infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Channel Count"));
        infoItem->setText(1, QString::number(XDFdata->streams[i].info.channel_count));

        infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Channel Format"));
        infoItem->setText(1, QString::fromStdString(XDFdata->streams[i].info.channel_format));
    }

    ui->spinBox->setMinimum(1);
    ui->spinBox->setValue(nativeSrate);
    if (highestSampleRate > 0)
        ui->spinBox->setMaximum(highestSampleRate);
    else
        ui->spinBox->setMaximum(2048);
    ui->treeWidget->expandAll();
}

ResamplingDialog::~ResamplingDialog()
{
    delete ui;
}

void ResamplingDialog::on_resample_button_clicked()
{
    userSrate = ui->spinBox->value();
    done(QDialog::Accepted);
}

void ResamplingDialog::on_cancel_button_clicked()
{
    done(QDialog::Rejected);
}

}
