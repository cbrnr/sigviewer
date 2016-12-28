// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "basic_header_info_dialog.h"
#include "file_handling/basic_header.h"
#include "file_handling_impl/xdf_reader.h"

#include <cmath>
#include <algorithm>

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
    resize(850, 850);
    top_layout->activate();
    connect(close_button_, SIGNAL(clicked()), this, SLOT(accept()));
}

// load settings
void BasicHeaderInfoDialog::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("BasicHeaderInfoDialog");
    resize(settings.value("size", QSize(850, 850)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    info_tree_widget_->header()->resizeSection(0, settings.value("col0_width", 400).toInt());
    info_tree_widget_->header()->resizeSection(1, settings.value("col1_width", 450).toInt());

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
    info_tree_widget_->setColumnWidth(0, this->width()/1.6);

    QTreeWidgetItem* root_item;
    QTreeWidgetItem* tmp_item;

    // basic
    root_item = new QTreeWidgetItem(info_tree_widget_);
    root_item->setText(0, tr("Basic"));
    //root_item->setIcon(0, QIcon(":/images/sigviewer16.png"));

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
    tmp_item = new QTreeWidgetItem(root_item);
    tmp_item->setText(0, tr("Size"));
    QFileInfo file_info (basic_header_->getFilePath());
    tmp_item->setText(1, QString::number(file_info.size() / 1024).append(tr(" KB")));
    tmp_item->setText(0, tr("File Size"));

    // events
    root_item = new QTreeWidgetItem(info_tree_widget_);
    root_item->setText(0, tr("Events"));
    root_item->setIcon(0, QIcon(":/images/events_22x22.png"));
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
        for (size_t i =0; i < XDFdata.streams.size(); i++)
        {
            // basic
            root_item = new QTreeWidgetItem(info_tree_widget_);
            root_item->setText(0, "Stream "+QString::number(i));
            //root_item->setIcon(0, QIcon(":/images/info_16x16.png"));
            info_tree_widget_->setAnimated(true);

            for (auto const &entry : XDFdata.streams[i].info.infoMap)
            {
                tmp_item = new QTreeWidgetItem(root_item);
                tmp_item->setText(0, QString::fromStdString(entry.first));
                tmp_item->setText(1, QString::fromStdString(entry.second));
            }

            tmp_item = new QTreeWidgetItem(root_item);
            tmp_item->setText(0, tr("desc"));

            if (XDFdata.streams[i].info.desc.provider.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("provider"));

                for (auto const &entry : XDFdata.streams[i].info.desc.provider)
                {
                    QTreeWidgetItem* provider_item = new QTreeWidgetItem(desc_item);
                    provider_item->setText(0, QString::fromStdString(entry.first));
                    provider_item->setText(1, QString::fromStdString(entry.second));
                }
            }

            if (XDFdata.streams[i].info.desc.facility.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("facility"));

                for (auto const &entry : XDFdata.streams[i].info.desc.facility)
                {
                    QTreeWidgetItem* facility_item = new QTreeWidgetItem(desc_item);
                    facility_item->setText(0, QString::fromStdString(entry.first));
                    facility_item->setText(1, QString::fromStdString(entry.second));
                }
            }

            if (XDFdata.streams[i].info.desc.synchronization.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("synchronization"));

                for (auto const &entry : XDFdata.streams[i].info.desc.synchronization)
                {
                    QTreeWidgetItem* synchronization_item = new QTreeWidgetItem(desc_item);
                    synchronization_item->setText(0, QString::fromStdString(entry.first));
                    synchronization_item->setText(1, QString::fromStdString(entry.second));
                }
            }

            if (XDFdata.streams[i].info.desc.encoding.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("encoding"));

                for (auto const &entry : XDFdata.streams[i].info.desc.encoding)
                {
                    QTreeWidgetItem* encoding_item = new QTreeWidgetItem(desc_item);
                    encoding_item->setText(0, QString::fromStdString(entry.first));
                    encoding_item->setText(1, QString::fromStdString(entry.second));
                }
            }

            if (XDFdata.streams[i].info.desc.acquisition.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("acquisition"));

                for (auto const &entry : XDFdata.streams[i].info.desc.acquisition)
                {
                    QTreeWidgetItem* acquisition_item = new QTreeWidgetItem(desc_item);
                    acquisition_item->setText(0, QString::fromStdString(entry.first));
                    acquisition_item->setText(1, QString::fromStdString(entry.second));
                }
            }

            if (XDFdata.streams[i].info.desc.reference.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("reference"));

                for (auto const &entry : XDFdata.streams[i].info.desc.reference)
                {
                    QTreeWidgetItem* reference_item = new QTreeWidgetItem(desc_item);
                    reference_item->setText(0, QString::fromStdString(entry.first));
                    reference_item->setText(1, QString::fromStdString(entry.second));
                }
            }

            if (XDFdata.streams[i].info.desc.cap.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("cap"));

                for (auto const &entry : XDFdata.streams[i].info.desc.cap)
                {
                    QTreeWidgetItem* cap_item = new QTreeWidgetItem(desc_item);
                    cap_item->setText(0, QString::fromStdString(entry.first));
                    cap_item->setText(1, QString::fromStdString(entry.second));
                }
            }

            if (XDFdata.streams[i].info.desc.location_measurement.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("location_measurement"));

                for (auto const &entry : XDFdata.streams[i].info.desc.location_measurement)
                {
                    QTreeWidgetItem* location_measurement_item = new QTreeWidgetItem(desc_item);
                    location_measurement_item->setText(0, QString::fromStdString(entry.first));
                    location_measurement_item->setText(1, QString::fromStdString(entry.second));
                }
            }

            if (XDFdata.streams[i].info.desc.amplifier.settings.size())
            {
                QTreeWidgetItem* desc_item = new QTreeWidgetItem(tmp_item);
                desc_item->setText(0, tr("amplifier"));

                QTreeWidgetItem* sub_item = new QTreeWidgetItem(desc_item);
                sub_item->setText(0, tr("settings"));

                for (auto const &entry : XDFdata.streams[i].info.desc.amplifier.settings)
                {
                    QTreeWidgetItem* settings_item = new QTreeWidgetItem(sub_item);
                    settings_item->setText(0, QString::fromStdString(entry.first));
                    settings_item->setText(1, QString::fromStdString(entry.second));
                }
            }


            if (!XDFdata.streams[i].time_series.empty())
            {
                // channels
                QTreeWidgetItem* channels_item;
                channels_item = new QTreeWidgetItem(root_item);
                channels_item->setText(0, tr("channels"));
                //channels_item->setIcon(0, QIcon(":/images/channels_22x22.png"));

                for (int32 channel_nr = 0;
                     channel_nr < XDFdata.streams[i].info.channel_count;
                     channel_nr++)
                {
                    QTreeWidgetItem* channel_item;

                    int channelIndex = std::distance(XDFdata.streamMap.begin(),
                                                     std::find(XDFdata.streamMap.begin(),XDFdata.streamMap.end(),i)) + channel_nr;

                    QSharedPointer<SignalChannel const> channel = basic_header_->getChannel (channelIndex);
                    channel_item = new QTreeWidgetItem(channels_item);
                    channel_item->setText(0, QString("%1").arg(channel->getLabel()));

                    if (XDFdata.streams[i].info.desc.channels.size())
                    {
                        for (auto const &entry : XDFdata.streams[i].info.desc.channels[channel_nr].channelInfoMap)
                        {
                            QTreeWidgetItem* tmp_item = new QTreeWidgetItem(channel_item);
                            tmp_item->setText(0, QString::fromStdString(entry.first));
                            tmp_item->setText(1, QString::fromStdString(entry.second));

                            if (entry.first.compare("location")==0)
                            {
                                for (auto const &subentry : XDFdata.streams[i].info.desc.channels[channel_nr].location)
                                {
                                    QTreeWidgetItem* location_item = new QTreeWidgetItem(tmp_item);
                                    location_item->setText(0, QString::fromStdString(subentry.first));
                                    location_item->setText(1, QString::fromStdString(subentry.second));
                                }
                            }

                            if (entry.first.compare("hardware")==0)
                            {
                                for (auto const &subentry : XDFdata.streams[i].info.desc.channels[channel_nr].hardware)
                                {
                                    QTreeWidgetItem* hardware_item = new QTreeWidgetItem(tmp_item);
                                    hardware_item->setText(0, QString::fromStdString(subentry.first));
                                    hardware_item->setText(1, QString::fromStdString(subentry.second));
                                }
                            }

                            if (entry.first.compare("filtering")==0)
                            {
                                if (XDFdata.streams[i].info.desc.channels[channel_nr].filtering.highpass.size())
                                {
                                    QTreeWidgetItem* highpass_item = new QTreeWidgetItem(tmp_item);
                                    highpass_item->setText(0, tr("highpass"));

                                    for (auto const &subentry : XDFdata.streams[i].info.desc.channels[channel_nr].filtering.highpass)
                                    {
                                        QTreeWidgetItem* buff_item = new QTreeWidgetItem(highpass_item);
                                        buff_item->setText(0, QString::fromStdString(subentry.first));
                                        buff_item->setText(1, QString::fromStdString(subentry.second));
                                    }
                                }

                                if (XDFdata.streams[i].info.desc.channels[channel_nr].filtering.lowpass.size())
                                {
                                    QTreeWidgetItem* lowpass_item = new QTreeWidgetItem(tmp_item);
                                    lowpass_item->setText(0, tr("lowpass"));

                                    for (auto const &subentry : XDFdata.streams[i].info.desc.channels[channel_nr].filtering.highpass)
                                    {
                                        QTreeWidgetItem* buff_item = new QTreeWidgetItem(lowpass_item);
                                        buff_item->setText(0, QString::fromStdString(subentry.first));
                                        buff_item->setText(1, QString::fromStdString(subentry.second));
                                    }
                                }

                                if (XDFdata.streams[i].info.desc.channels[channel_nr].filtering.notch.size())
                                {
                                    QTreeWidgetItem* notch_item = new QTreeWidgetItem(tmp_item);
                                    notch_item->setText(0, tr("notch"));

                                    for (auto const &subentry : XDFdata.streams[i].info.desc.channels[channel_nr].filtering.highpass)
                                    {
                                        QTreeWidgetItem* buff_item = new QTreeWidgetItem(notch_item);
                                        buff_item->setText(0, QString::fromStdString(subentry.first));
                                        buff_item->setText(1, QString::fromStdString(subentry.second));
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (XDFdata.streams[i].info.desc.fiducials.size())
            {
                //fiducials
                QTreeWidgetItem* fiducials_item = new QTreeWidgetItem(root_item);
                fiducials_item->setText(0, tr("fiducials"));


                for (auto const &fiducial : XDFdata.streams[i].info.desc.fiducials)
                {
                    QTreeWidgetItem* fiducial_item = new QTreeWidgetItem(fiducials_item);
                    fiducial_item->setText(0, tr("fiducial"));

                    QTreeWidgetItem* label_item = new QTreeWidgetItem(fiducial_item);
                    label_item->setText(0, tr("label"));

                    if (fiducial.location.size())
                    {
                        QTreeWidgetItem* fiducialLocation_item = new QTreeWidgetItem(fiducial_item);
                        fiducialLocation_item->setText(0, tr("location"));

                        for (auto const &entry : fiducial.location)
                        {
                            QTreeWidgetItem* fiducialLocationChild_item = new QTreeWidgetItem(fiducialLocation_item);
                            fiducialLocationChild_item->setText(0, QString::fromStdString(entry.first));
                            fiducialLocationChild_item->setText(1, QString::fromStdString(entry.second));
                        }
                    }
                }
            }
        }
    }   //XDF ends here
    else
    {
        // patient
        root_item = new QTreeWidgetItem(info_tree_widget_);
        root_item->setText(0, tr("Patient"));
        root_item->setIcon(0, QIcon(":/images/patient_16x16.png"));
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
        root_item->setIcon(0, QIcon(":/images/channels_22x22.png"));

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
    info_tree_widget_->expandAll();
}

}
