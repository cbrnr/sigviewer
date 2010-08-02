/*

    $Id: file_signal_reader_factory.cpp,v 1.18 2009-03-10 07:47:58 schloegl Exp $
    Copyright (C) Thomas Brunner  2005
    		  Christoph Eibel 2008
		  Clemens Brunner 2005,2008
    		  Alois Schloegl  2009,2010
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

#include "file_signal_reader_factory.h"

namespace SigViewer_
{

//-------------------------------------------------------------------------
std::auto_ptr<FileSignalReaderFactory> FileSignalReaderFactory::instance_;

//-------------------------------------------------------------------------
FileSignalReaderFactory* FileSignalReaderFactory::getInstance ()
{
    if (!instance_.get())
    {
        instance_.reset(new FileSignalReaderFactory);
    }
    return instance_.get();
}

} // namespace SigViewer_
