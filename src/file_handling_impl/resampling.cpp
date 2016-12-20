#include "resampling.h"
#include "ui_resampling.h"
#include "xdf_reader.h"

namespace SigViewer_ {


Resampling::Resampling(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Resampling)
{
    ui->setupUi(this);
}

Resampling::Resampling(int nativeSrate, int highestSampleRate, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Resampling)
{
    ui->setupUi(this);
    this->setWindowTitle("Resampling");
    if (XDFdata.sampleRateMap.size() > 1)
    {
        QString text = "This file contains multiple sample rates across various streams.<br> "
                       "Sigviewer needs to resample some channels to a unified sample rate in order to display them.<br> "
                       "Please choose a sample rate below (This won't change the actual file content):";
        ui->label->setText(text);
    }
    else
    {
        QString text = "Would you like to resample this file? (This won't change the actual file content) ";
        ui->label->setText(text);
    }
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setColumnWidth(0, 270);
    ui->treeWidget->setColumnWidth(1, 270);
    QStringList headers;
    headers << "Stream" << "Info";
    ui->treeWidget->setHeaderLabels(headers);
    for (size_t i = 0; i < XDFdata.streams.size(); i++)
    {
        QTreeWidgetItem* streamItem = new QTreeWidgetItem(ui->treeWidget);
        streamItem->setExpanded(true);
        streamItem->setText(0, "Stream "+QString::number(i));

        QTreeWidgetItem* infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Name"));
        infoItem->setText(1, QString::fromStdString(XDFdata.streams[i].info.infoMap["name"]));

        infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Type"));
        infoItem->setText(1, QString::fromStdString(XDFdata.streams[i].info.infoMap["type"]));

        infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Sample Rate"));
        infoItem->setText(1, QString::number(XDFdata.streams[i].info.nominal_srate).append(tr(" Hz")));

        infoItem = new QTreeWidgetItem(streamItem);
        infoItem->setText(0, tr("Channel Format"));
        infoItem->setText(1, QString::fromStdString(XDFdata.streams[i].info.infoMap["channel_format"]));
    }

    ui->spinBox->setValue(nativeSrate);
    ui->spinBox->setMaximum(highestSampleRate);
}


Resampling::~Resampling()
{
    delete ui;
}

void Resampling::on_buttonBox_accepted()
{
    userSrate = ui->spinBox->value();
}

void SigViewer_::Resampling::on_buttonBox_rejected()
{
    cancelled = true;
}

}
