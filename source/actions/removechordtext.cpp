/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "removechordtext.h"

#include <powertabdocument/system.h>

using boost::shared_ptr;

RemoveChordText::RemoveChordText(shared_ptr<System> system, quint32 index) :
    system(system),
    index(index)
{
    setText(QObject::tr("Remove Chord Text"));

    chordText = system->GetChordText(index);
}

void RemoveChordText::redo()
{
    system->RemoveChordText(index);
}

void RemoveChordText::undo()
{
    system->InsertChordText(chordText, index);
}
