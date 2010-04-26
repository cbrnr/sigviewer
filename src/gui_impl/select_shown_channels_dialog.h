#ifndef SELECT_SHOWN_CHANNELS_DIALOG_H
#define SELECT_SHOWN_CHANNELS_DIALOG_H

#include "../base/user_types.h"

#include <QDialog>

#include <set>

class QListWidget;
class QPushButton;

namespace BioSig_
{

class SelectShownChannelsDialog : public QDialog
{
    Q_OBJECT
public:
    SelectShownChannelsDialog (QString const& dialog_title,
                               std::set<ChannelID> const& channels);

    std::set<ChannelID> getSelectedChannels () const;

private:
    SelectShownChannelsDialog ();
    SelectShownChannelsDialog (const SelectShownChannelsDialog&);
    const SelectShownChannelsDialog& operator= (const SelectShownChannelsDialog&);

    std::set<ChannelID> channels_;
    QListWidget* channel_list_widget_;
    QPushButton* ok_button_;
    QPushButton* cancel_button_;
};

} // namespace BioSig_

#endif // SELECT_SHOWN_CHANNELS_DIALOG_H
