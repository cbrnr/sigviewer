// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "basic_header_info_dialog.h"
#include "file_handling/basic_header.h"
#include "file_handling_impl/xdf_reader.h"

#include <cmath>
#include <algorithm>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSettings>
#include <QFileInfo>
#include <QtXml>
#include <QDesktopWidget>

namespace sigviewer
{

// constructor
BasicHeaderInfoDialog::BasicHeaderInfoDialog(QSharedPointer<BasicHeader> header,
                                             QWidget* parent)
    : QDialog(parent),
      basic_header_(header)
{
    setWindowTitle(tr("Basic Header Info"));
    QVBoxLayout* top_layout = new QVBoxLayout(this);
    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    info_tree_widget_ = new QTreeWidget(this);
    top_layout->addWidget(info_tree_widget_);
    QHBoxLayout* button_layout = new QHBoxLayout;
    button_layout->setMargin(0);
    top_layout->addLayout(button_layout);
    button_layout->addStretch(1);
    close_button_ = new QPushButton(tr("Close"), this);
    toggle_button_ = new QPushButton(tr("Collapse All"), this);
    button_layout->addWidget(toggle_button_);
    button_layout->addWidget(close_button_);
    button_layout->addStretch(1);

    QDesktopWidget dw;
    int x=dw.width()*0.45;
    int y=dw.height()*0.8;
    resize(x,y);

    buildTree();

    top_layout->activate();
    readSettings();
    connect(close_button_, SIGNAL(clicked()), this, SLOT(closeInfoDialog()));
    connect(this, SIGNAL(finished(int)), this, SLOT(closeInfoDialog()));
    connect(toggle_button_, SIGNAL(clicked()), this, SLOT(toggleCollapseExpand()));
//    connect(info_tree_widget_, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(showStreamName(QTreeWidgetItem*)));
//    connect(info_tree_widget_, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(hideStreamName(QTreeWidgetItem*)));
}

void BasicHeaderInfoDialog::toggleCollapseExpand()
{
    if (toggle_button_->text().compare("Collapse All") == 0)
    {
        info_tree_widget_->collapseAll();
        toggle_button_->setText("Expand All");
    }
    else if (toggle_button_->text().compare("Expand All") == 0)
    {
        info_tree_widget_->expandAll();
        toggle_button_->setText("Collapse All");
    }
}

void BasicHeaderInfoDialog::showStreamName(QTreeWidgetItem *item)
{
    if (item->text(0).startsWith("Stream", Qt::CaseInsensitive))
    {
        QRegExp rx("(\\d+)");
        int pos = rx.indexIn(item->text(0));
        QString str;
        if (pos > -1)
        {
            str = rx.cap(1);
        }
        int streamNumber = str.toInt() - 1; //-1 to switch back to 0-based indexing

//        int streamNumber = item->text(0).remove("Stream ").toInt() - 1;//-1 to switch back to 0 index
        item->setText(1, QString::fromStdString(XDFdata->streams[streamNumber].info.name));
    }
}

void BasicHeaderInfoDialog::hideStreamName(QTreeWidgetItem *item)
{
    if (item->text(0).startsWith("Stream", Qt::CaseInsensitive))
    {
        item->setText(1, "");
    }
}

void BasicHeaderInfoDialog::closeInfoDialog()
{
    QSettings settings;
    settings.beginGroup("BasicHeaderInfoDialog");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    close();
}

void BasicHeaderInfoDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("BasicHeaderInfoDialog");
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

// build tree
void BasicHeaderInfoDialog::buildTree()
{
    info_tree_widget_->setIconSize(QSize(16, 16));
    info_tree_widget_->setRootIsDecorated(true);
    QStringList header_labels;
    header_labels << tr("Property") << tr("Value");
    info_tree_widget_->setHeaderLabels(header_labels);

    info_tree_widget_->header()->setSectionResizeMode(QHeaderView::Interactive);
    info_tree_widget_->setColumnWidth(0, width() * 0.5);
    info_tree_widget_->setAnimated(true);

    QTreeWidgetItem* root_item;
    QTreeWidgetItem* tmp_item;

    // basic
    root_item = new QTreeWidgetItem(info_tree_widget_);
    root_item->setText(0, tr("Basic"));
//    root_item->setIcon(0, QIcon(":/images/ic_info_outline_black_24dp.png"));

    tmp_item = new QTreeWidgetItem(root_item);
    tmp_item->setText(0, tr("File Type"));
    tmp_item->setText(1, basic_header_->getFileTypeString());

    QMap<QString, QString> recording_info = basic_header_->getRecordingInfo();
    foreach (QString key, recording_info.keys())
    {
        tmp_item = new QTreeWidgetItem(root_item);
        tmp_item->setText(0, key);
        tmp_item->setText(1, recording_info[key]);
    }


    //    tmp_item = new QTreeWidgetItem(root_item);
    //    tmp_item->setText(0, tr("Recording Time"));
    //    tmp_item->setText(1, basic_header_->getRecordingTime().toString("dd.MM.yyyy hh:mm:ss"));
    //    tmp_item = new QTreeWidgetItem(root_item);
    //    // tmp_item ->setTextAlignment(1, Qt::AlignRight);
    //    tmp_item->setText(0, tr("Triggered"));
    //    tmp_item->setText(1, basic_header_->isTriggered() ? tr("yes") : tr("no"));
    //    tmp_item = new QTreeWidgetItem(root_item);
    //    // tmp_item ->setTextAlignment(1, Qt::AlignRight);
    //    tmp_item->setText(0, tr("Recording"));
    //    tmp_item->setText(1, QString::number(basic_header_->getRecordDuration() *
    //                                      basic_header_->getNumberRecords()));
    //    tmp_item->setText(2, tr("seconds"));

    // file
    root_item = new QTreeWidgetItem(info_tree_widget_);
    root_item->setText(0, tr("File"));
//    root_item->setIcon(0, QIcon(":/images/ic_folder_open_black_24dp.png"));
    tmp_item = new QTreeWidgetItem(root_item);
    tmp_item->setText(0, tr("Size"));
    QFileInfo file_info (basic_header_->getFilePath());
    tmp_item->setText(1, QString::number(file_info.size() / 1024).append(tr(" KB")));
    tmp_item->setText(0, tr("File Size"));

    // events
    root_item = new QTreeWidgetItem(info_tree_widget_);
    root_item->setText(0, tr("Events"));
//    root_item->setIcon(0, QIcon(":/images/ic_font_download_black_24dp.png"));
    tmp_item = new QTreeWidgetItem(root_item);
    // tmp_item ->setTextAlignment(1, Qt::AlignRight);
    tmp_item->setText(0, tr("Number"));
    tmp_item->setText(1, QString::number(basic_header_->getNumberEvents()).append(tr(" events")));
    tmp_item = new QTreeWidgetItem(root_item);
    // tmp_item ->setTextAlignment(1, Qt::AlignRight);
    tmp_item->setText(0, tr("Sample Rate"));
    tmp_item->setText(1, QString::number(basic_header_->getEventSamplerate()).append(tr(" Hz")));

    //exclusively for XDF
    if (basic_header_->getFileTypeString().startsWith("XDF", Qt::CaseInsensitive))
    {
        for (size_t i =0; i < XDFdata->streams.size(); i++)
        {
            // basic
            root_item = new QTreeWidgetItem(info_tree_widget_);
//            root_item->setText(0, "Stream "+QString::number(i + 1));//+1 for user's convenience (1 based instead 0 based)
            root_item->setText(0, QString("Stream %1 (%2)").arg(QString::number(i+1)).        //+1 for 1-based indexing
                                   arg(QString::fromStdString(XDFdata->streams[i].info.name)));

//            root_item->setIcon(0, QIcon(":/images/ic_flag_black_24dp.png"));

            QDomDocument streamHeader;
            streamHeader.setContent(QString::fromStdString(XDFdata->streams[i].streamHeader));
            QDomElement rootElement = streamHeader.firstChildElement();

            for (QDomNode n = rootElement.firstChild(); !n.isNull();)
            {
                tmp_item = new QTreeWidgetItem(root_item);
                tmp_item->setText(0, n.nodeName());
                if (!n.firstChild().isElement())
                {
                    tmp_item->setText(1, n.toElement().text());
                    while (n.nextSibling().isNull() &&
                           n.parentNode() != rootElement)
                    {
                        n = n.parentNode();
                        tmp_item = tmp_item->parent();
                        root_item = tmp_item->parent();
                    }
                    n = n.nextSibling();
                }
                else
                {
                    n = n.firstChild();
                    root_item = tmp_item;
                }
            }

            QDomDocument streamFooter;
            streamFooter.setContent(QString::fromStdString(XDFdata->streams[i].streamFooter));
            rootElement = streamFooter.firstChildElement();
            for (QDomNode n = rootElement.firstChild(); !n.isNull();)
            {
                tmp_item = new QTreeWidgetItem(root_item);
                tmp_item->setText(0, n.nodeName());
                if (!n.firstChild().isElement())
                {
                    tmp_item->setText(1, n.toElement().text());
                    while (n.nextSibling().isNull() &&
                           n.parentNode() != rootElement)
                    {
                        n = n.parentNode();
                        tmp_item = tmp_item->parent();
                        root_item = tmp_item->parent();
                    }
                    n = n.nextSibling();
                }
                else
                {
                    n = n.firstChild();
                    root_item = tmp_item;
                }
            }

        }
    }   //XDF ends here
    else
    {
        // patient
        root_item = new QTreeWidgetItem(info_tree_widget_);
        root_item->setText(0, tr("Patient"));
//        root_item->setIcon(0, QIcon(":/images/patient_16x16.png"));
        QMap<QString, QString> patient_info = basic_header_->getPatientInfo();
        foreach (QString key, patient_info.keys())
        {
            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, key);
            tmp_item->setText(1, patient_info[key]);
        }

        // channels
        root_item = new QTreeWidgetItem(info_tree_widget_);
        root_item->setText(0, tr("Channels"));
//        root_item->setIcon(0, QIcon(":/images/channels_22x22.png"));

        for (uint32 channel_nr = 0;
             channel_nr < basic_header_->getNumberChannels();
             channel_nr++)
        {
            QTreeWidgetItem* channel_item;
            QSharedPointer<SignalChannel const> channel = basic_header_->getChannel (channel_nr);
            channel_item = new QTreeWidgetItem(root_item);

//            QRegExp rx("(^channel\\s*\\d+)");
//            rx.setCaseSensitivity(Qt::CaseInsensitive);

//            if (channel->getLabel().contains(rx))
//                channel_item->setText(0, channel->getLabel());
//            else
                channel_item->setText(0, QString("Channel %1 (%2)").arg(channel_nr + 1) // +1 for 1-based indexing
                                  .arg(channel->getLabel()));

            // channel basic
            tmp_item = new QTreeWidgetItem(channel_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Label"));
            tmp_item->setText(1, channel->getLabel());
            tmp_item = new QTreeWidgetItem(channel_item);

            tmp_item->setText(0, tr("Sample Rate"));
            float64 fs = channel->getSampleRate();
            if (fs < 0.0)
                fs = basic_header_->getSampleRate();
            tmp_item->setText(1, QString::number(fs).append(tr(" Hz")));

            tmp_item = new QTreeWidgetItem(channel_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Physical Dimension"));
            tmp_item->setText(1, channel->getPhysicalDim());
            tmp_item = new QTreeWidgetItem(channel_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Physical Maximum"));
            tmp_item->setText(1, QString::number(channel->getPhysicalMaximum()));
            tmp_item = new QTreeWidgetItem(channel_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Physical Minimum"));
            tmp_item->setText(1, QString::number(channel->getPhysicalMinimum()));
            tmp_item = new QTreeWidgetItem(channel_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Digital Maximum"));
            tmp_item->setText(1, QString::number(channel->getDigitalMaximum()));
            tmp_item = new QTreeWidgetItem(channel_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Digital Minimum"));
            tmp_item->setText(1, QString::number(channel->getDigitalMinimum()));
            tmp_item = new QTreeWidgetItem(channel_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Data Type"));
            tmp_item->setText(1, channel->typeString());

            // filter
            QTreeWidgetItem* filter_item;
            filter_item = new QTreeWidgetItem(channel_item);
            filter_item->setText(0, tr("Filter"));
            tmp_item = new QTreeWidgetItem(filter_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Highpass"));
            //tmp_item->setText(1, isnan(channel->getHighpass()) ? tr("unknown") :
            //    (channel->getHighpass() < 0 ? "" :
            //                       QString::number(channel->getHighpass())));
            tmp_item->setText(2, tr("Hz"));
            tmp_item = new QTreeWidgetItem(filter_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Lowpass"));
            // tmp_item->setText(1, channel->getLowpass() ? tr("unknown") :
            //                (channel->getLowpass() < 0 ? "" :
            //                           QString::number(channel->getLowpass())));
            tmp_item->setText(2, tr("Hz"));
            tmp_item = new QTreeWidgetItem(filter_item);
            // tmp_item ->setTextAlignment(1, Qt::AlignRight);
            tmp_item->setText(0, tr("Notch"));
            //tmp_item->setText(1, isnan(channel->getNotch()) ? tr("unknown") : (channel->getNotch() ? tr("yes") : tr("no")));
        }
    }    
    info_tree_widget_->expandAll();
}

}
