// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef BASIC_HEADER_INFO_DIALOG_H
#define BASIC_HEADER_INFO_DIALOG_H

#include "file_handling/basic_header.h"

#include <QDialog>
#include <QSharedPointer>

class QTreeWidget;
class QPushButton;

namespace sigviewer
{

// basic header info dialog
class BasicHeaderInfoDialog : public QDialog
{
    Q_OBJECT
public:
    BasicHeaderInfoDialog(QSharedPointer<BasicHeader> header, QWidget* parent = 0);

    void loadSettings();
    void saveSettings();

private:
    // not allowed
    BasicHeaderInfoDialog();
    BasicHeaderInfoDialog(const BasicHeaderInfoDialog&);
    const BasicHeaderInfoDialog& operator=(const BasicHeaderInfoDialog&);

    void buildTree();

    QSharedPointer<BasicHeader> basic_header_;
    QTreeWidget* info_tree_widget_;
    QPushButton* close_button_;
};

} // namespace SigViewer_

#endif
