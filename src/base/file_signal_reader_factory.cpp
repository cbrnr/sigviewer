/*

    $Id: file_signal_reader_factory.cpp,v 1.18 2009-03-10 07:47:58 schloegl Exp $
    Copyright (C) Thomas Brunner  2005
    		  Christoph Eibel 2008
		  Clemens Brunner 2005,2008
    		  Alois Schloegl  2009
    This file is part of the "SigViewer" repository
    at http://biosig.sf.net/

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


// file_signal_reader_factory.h

#include "file_signal_reader_factory.h"

// all suported readers
#include "biosig_reader.h"

namespace BioSig_
{

// instance
std::auto_ptr<FileSignalReaderFactory> FileSignalReaderFactory::instance_;

// get instance
FileSignalReaderFactory* FileSignalReaderFactory::getInstance()
{
    if (!instance_.get())
    {
        instance_.reset(new FileSignalReaderFactory);

        // register all readers
		instance_->addPrototype(".gdf", new BioSigReader);
        instance_->addPrototype(".evt", new BioSigReader);
       	instance_->addPrototype(".bdf", new BioSigReader);
        instance_->addPrototype(".bkr", new BioSigReader);
        instance_->addPrototype(".cnt", new BioSigReader);
        instance_->addPrototype(".edf", new BioSigReader);
        instance_->addPrototype(".eeg", new BioSigReader);
		instance_->setDefaultPrototype(new BioSigReader);
    }
    return instance_.get();
}

// get extensions
QString FileSignalReaderFactory::getExtensions()
{
    QString extensions;
    QStringList extension_list = getElementNames();
    foreach(QString extension, extension_list)
    {
        if (extension.startsWith("."))
        {
            extensions += "*" + extension + " ";
        }
    }
    if (extensions.length() > 0)
    {
        extensions = extensions.left(extensions.length() - 1);
    }
    return extensions;
}

} // namespace BioSig_
