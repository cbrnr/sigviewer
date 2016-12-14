// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "basic_header_info_dialog.h"
#include "file_handling/basic_header.h"
#include "file_handling_impl/xdf_reader.h"

#include <cmath>

#include <QTreeWidget>
#include <QDateTime>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSettings>
#include <QFileInfo>

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
    button_layout->addWidget(close_button_);
    button_layout->addStretch(1);
    buildTree();
    resize(400, 400);
    top_layout->activate();
    connect(close_button_, SIGNAL(clicked()), this, SLOT(accept()));
}

// load settings
void BasicHeaderInfoDialog::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("BasicHeaderInfoDialog");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    info_tree_widget_->header()->resizeSection(0, settings.value("col0_width", 100).toInt());
    info_tree_widget_->header()->resizeSection(1, settings.value("col1_width", 150).toInt());

    settings.endGroup();
}

// save settings
void BasicHeaderInfoDialog::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("BasicHeaderInfoDialog");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("col0_width", info_tree_widget_->header()->sectionSize(0));
    settings.setValue("col1_width", info_tree_widget_->header()->sectionSize(1));

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

    QTreeWidgetItem* root_item;
    QTreeWidgetItem* tmp_item;

    // basic
    root_item = new QTreeWidgetItem(info_tree_widget_);
    root_item->setText(0, tr("Basic"));
    root_item->setIcon(0, QIcon(":/images/info_16x16.png"));
    info_tree_widget_->setItemExpanded(root_item, true);

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
    root_item->setIcon(0, QIcon(":/images/file_16x16.png"));
    info_tree_widget_->setItemExpanded(root_item, true);
    tmp_item = new QTreeWidgetItem(root_item);
    tmp_item->setText(0, tr("Size"));
    QFileInfo file_info (basic_header_->getFilePath());
    tmp_item->setText(1, QString::number(file_info.size() / 1024).append(tr(" KB")));
    tmp_item->setText(0, tr("File Size"));

    // patient
    root_item = new QTreeWidgetItem(info_tree_widget_);
    root_item->setText(0, tr("Patient"));
    root_item->setIcon(0, QIcon(":/images/patient_16x16.png"));
    info_tree_widget_->setItemExpanded(root_item, true);
    QMap<QString, QString> patient_info = basic_header_->getPatientInfo();
    foreach (QString key, patient_info.keys())
    {
        tmp_item = new QTreeWidgetItem(root_item);
        tmp_item->setText(0, key);
        tmp_item->setText(1, patient_info[key]);
    }

    // events
    root_item = new QTreeWidgetItem(info_tree_widget_);
    root_item->setText(0, tr("Events"));
    root_item->setIcon(0, QIcon(":/images/events_22x22.png"));
    info_tree_widget_->setItemExpanded(root_item, true);
    tmp_item = new QTreeWidgetItem(root_item);
    // tmp_item ->setTextAlignment(1, Qt::AlignRight);
    tmp_item->setText(0, tr("Number"));
    tmp_item->setText(1, QString::number(basic_header_->getNumberEvents()).append(tr(" events")));
    tmp_item = new QTreeWidgetItem(root_item);
    // tmp_item ->setTextAlignment(1, Qt::AlignRight);
    tmp_item->setText(0, tr("Sample Rate"));
    tmp_item->setText(1, QString::number(basic_header_->getEventSamplerate()).append(tr(" Hz")));

    //exclusively for XDF
    if (basic_header_->getFileTypeString().indexOf("XDF") == 0)
    {
        for (size_t i =0; i < XDFdata.streams.size(); i++)
        {
            // basic
            root_item = new QTreeWidgetItem(info_tree_widget_);
            root_item->setText(0, "Stream "+QString::number(i));
            root_item->setIcon(0, QIcon(":/images/info_16x16.png"));
            info_tree_widget_->setItemExpanded(root_item, true);

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("name"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.name));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("type"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.type));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("channel_count"));
            if (XDFdata.streams[i].info.channel_count <= 1)
            {
                tmp_item->setText(1, QString::number(XDFdata.streams[i].info.channel_count).append(tr(" channel")));
            }
            else
            {
                tmp_item->setText(1, QString::number(XDFdata.streams[i].info.channel_count).append(tr(" channels")));
            }

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("nominal_srate"));
            tmp_item->setText(1, QString::number(XDFdata.streams[i].info.nominal_srate).append(tr(" Hz")));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("channel_format"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.channel_format));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("source_id"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.source_id));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("version"));
            tmp_item->setText(1, QString::number(XDFdata.streams[i].info.version, 'f', 1));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("created_at"));
            tmp_item->setText(1, QString::number(XDFdata.streams[i].info.created_at));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("uid"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.uid));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("session_id"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.session_id));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("hostname"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.hostname));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("v4address"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.v4address));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("v4data_port"));
            tmp_item->setText(1, QString::number(XDFdata.streams[i].info.v4data_port));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("v4service_port"));
            tmp_item->setText(1, QString::number(XDFdata.streams[i].info.v4service_port));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("v6address"));
            tmp_item->setText(1, QString::fromStdString(XDFdata.streams[i].info.v6address));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("v6data_port"));
            tmp_item->setText(1, QString::number(XDFdata.streams[i].info.v6data_port));

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("v6service_port"));
            tmp_item->setText(1, QString::number(XDFdata.streams[i].info.v6service_port));

            if (!XDFdata.streams[i].time_series.empty())
            {
                // channels
                QTreeWidgetItem* XDFchannel;
                XDFchannel = new QTreeWidgetItem(root_item);
                XDFchannel->setText(0, tr("Channels"));
                XDFchannel->setIcon(0, QIcon(":/images/channels_22x22.png"));
                info_tree_widget_->setItemExpanded(XDFchannel, true);

                for (uint32 channel_nr = 0;
                     channel_nr < XDFdata.streams[i].info.channel_count;
                     channel_nr++)
                {
                    QTreeWidgetItem* channel_item;
                    int streamMapIndex;
                    for (size_t str = 0; str < XDFdata.streamMap.size(); str++)
                    {
                        if (XDFdata.streamMap[str].first == i)
                        {
                            streamMapIndex = str;
                            break;
                        }
                    }
                    int channelIndex = XDFdata.streamMap[streamMapIndex].second - XDFdata.streams[i].info.channel_count + channel_nr;
                    QSharedPointer<SignalChannel const> channel = basic_header_->getChannel (channelIndex);
                    channel_item = new QTreeWidgetItem(XDFchannel);
                    channel_item->setText(0, QString("(%1) %2").arg(channelIndex)
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
        }
    }   //XDF ends here
    else
    {
        // channels
        root_item = new QTreeWidgetItem(info_tree_widget_);
        root_item->setText(0, tr("Channels"));
        root_item->setIcon(0, QIcon(":/images/channels_22x22.png"));
        info_tree_widget_->setItemExpanded(root_item, true);

        for (uint32 channel_nr = 0;
             channel_nr < basic_header_->getNumberChannels();
             channel_nr++)
        {
            QTreeWidgetItem* channel_item;
            QSharedPointer<SignalChannel const> channel = basic_header_->getChannel (channel_nr);
            channel_item = new QTreeWidgetItem(root_item);
            channel_item->setText(0, QString("(%1) %2").arg(channel_nr + 1)
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
}

}
